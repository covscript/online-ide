# Running with CovScript Online IDE Backends
# Setting up a crontab job to remove expired records to avoid exploding database.

import codec.json as json
import database
import regex

# CSDBC

namespace csdbc
    constant sql_int = "INT"
    constant sql_real = "REAL"
    constant sql_text = "TEXT"

    class statement_base
        function done()
            return true
        end
        function exec()
            return null
        end
        function just_exec()
        end
        function bind(idx, str)
        end
    end

    class database_base
        var typenames = null
        var typenames_full = null
        function prepare(sql)
            return null
        end
        function exec(sql)
            return null
        end
        function just_exec(sql)
        end
        function table_list()
            return null
        end
        function column_info(table)
            return null
        end
    end
end

struct record
    var name = null
    var data = null
end

struct column
    var name = null
    var type = null
    var data = new string
end

function convert(stmt)
    var data=new array
    while !stmt.done()
        var column_count=stmt.column_count()-1
        var row=new array
        for i=0,i<=column_count,++i
            var dat=new record
            dat.name=stmt.column_name(i)
            dat.data=stmt.get(i)
            row.push_back(dat)
        end
        data.push_back(row)
    end
    return data
end

function table_list(db)
    var tables=convert(db.exec("SELECT TABLE_NAME FROM information_schema.TABLES"))
    var table_list=new array
    foreach row in tables
        table_list.push_back(row[0].data)
    end
    return move(table_list)
end

function column_info(db,table)
    var info=convert(db.exec("select COLUMN_NAME,COLUMN_TYPE from information_schema.COLUMNS where TABLE_NAME=\""+table+"\""))
    var filter=regex.build("^(\\w+)\\(.*\\)$")
    var res=new array
    foreach it in info
        var col=new column
        col.name=it[0].data
        col.type=filter.match(it[1].data).str(1)
        res.push_back(move(col))
    end
    return move(res)
end

@begin
constant mysql_typenames = {
    csdbc.sql_int:"INT",
    csdbc.sql_real:"FLOAT",
    csdbc.sql_text:"VARCHAR"
}.to_hash_map()
@end

@begin
constant mysql_typenames_full = {
    csdbc.sql_int:"INTEGER",
    csdbc.sql_real:"FLOAT",
    csdbc.sql_text:"VARCHAR(255)"
}.to_hash_map()
@end

class statement extends csdbc.statement_base
    var bind_cache = new array
    var stmt = null
    function exec() override
        return convert(stmt.exec())
    end
    function just_exec() override
        stmt.just_exec()
    end
    function bind(idx, val) override
        bind_cache[idx] = to_string(val)
        stmt.bind(idx, bind_cache[idx])
    end
end

class mysql extends csdbc.database_base
    var db = null
    function initialize()
        this.typenames = mysql_typenames
        this.typenames_full = mysql_typenames_full
    end
    function prepare(sql) override
        var stmt = new statement
        stmt.stmt = db.prepare(sql)
        return move(stmt)
    end
    function exec(sql) override
        return global.convert(db.exec(sql))
    end
    function just_exec(sql) override
        db.just_exec(sql)
    end
    function table_list() override
        return global.table_list(db)
    end
    function column_info(table) override
        return global.column_info(db, table)
    end
end

# Expires time(second)
var expire_time = 600

function get_time()
    var tm = runtime.utc_time()
    return {"year": tm.year, "yday": tm.yday, "hour": tm.hour, "min": tm.min, "sec": tm.sec}.to_hash_map()
end

function time_padding(time, width)
    var last = width - time.size
    if last <= 0
        return time
    end
    var str = new string
    foreach it in range(last) do str += "0"
    return str + time
end

function time2str(tm)
    return json.to_string(json.from_var(tm))
end

function record_expired(timestamp)
    var ts = json.to_var(json.from_string(timestamp))
    var cur = get_time()
    if ts.year != cur.year
        return true
    end
    if cur.yday - ts.yday > 1
        return true
    end
    if cur.yday > ts.yday
        cur.hour += 24
    end
    var diff = (cur.hour - ts.hour)*3600 + (cur.min - ts.min)*60 + (cur.sec - ts.sec)
    return diff > expire_time
end

function get_timestr()
    var str = to_string(runtime.utc_time())
    str.cut(1)
    return move(str)
end

# Database
# CREATE TABLE temp_codes(hash VARCHAR(255), code VARCHAR(255), stdin VARCHAR(255), time VARCHAR(255))

var conn_str = "Driver={DRIVER NAME};SERVER={SERVER NAME};USER={USER NAME};PASSWORD=12345678;DATABASE=wxacode;PORT=3306"
var db = new mysql; db.db = database.connect(conn_str)

var data = db.exec("select * from temp_codes")
var expired_hash = new hash_set

foreach col in data
    if record_expired(col[3].data)
        expired_hash.insert(col[0].data)
    end
end

if expired_hash.empty()
    system.out.println("[" + get_timestr() + "]: No record need to be removed.")
end

foreach hash in expired_hash
    db.just_exec("DELETE FROM temp_codes WHERE hash=\"" + hash + "\"")
    system.out.println("[" + get_timestr() + "]: Record \"" + hash + "\" was removed.")
end