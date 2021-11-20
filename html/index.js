function GetQueryString(name) {
    let reg = new RegExp('(^|&)' + name + '=([^&]*)(&|$)')
    let r = window.location.search.substr(1).match(reg)
    if (r != null) {
        return unescape(r[2])
    }
    return null
}

function onRun() {
    var code = document.getElementById("code").value
    var stdin = document.getElementById("stdin").value
    document.getElementById("result").value = ""

    var ecode = Base64.encode(code)
    var estdin = Base64.encode(stdin)

    $.ajax({
        type: "post",
        url: "https://dev.covariant.cn/cgi/cs-online",
        data: JSON.stringify({ code: ecode, stdin: estdin }),
        async: true,
        success: function (data) {
            document.getElementById("result").value = data
        },
        error: function (data) {
            document.getElementById("result").value = "Error: " + data
        }
    });
}

function resetDefault() {
    document.getElementById("stdin").value =
        "{\"username\":\"mikecovlee\", \"password\":\"helloworld\"}"
    document.getElementById("result").value = ""
    document.getElementById("code").value =
        "import codec.json as json\n\n" +
        "var c = json.to_var(json.from_string(system.in.getline()))\n\n" +
        "system.out.println(\"UserName = \" + c.username)\n" +
        "system.out.println(\"Password = \" + c.password)\n"
}

function onReset() {
    var page = GetQueryString("page")
    if (page != null) {
        document.getElementById("stdin").value = ""
        document.getElementById("result").value = ""
        $.ajax({
            type: "get",
            url: "https://csman.info/doku.php?id=" + page + "&do=export_raw",
            success: function (data) {
                document.getElementById("code").value = data
            },
            error: function (data) {
                resetDefault();
            }
        })
    } else {
        resetDefault();
    }
}