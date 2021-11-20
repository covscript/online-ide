var cs_online_ver = "20211120.1"

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
    $.ajax({
        type: "get",
        url: "https://dev.covariant.cn/cgi/cs-ver",
        async: true,
        success: function (data) {
            document.getElementById("covscript_ver").innerHTML = "Backends: " + cs_online_ver + " CovScript/" + data
        }
    })
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

function genWxaCode() {
    var code = document.getElementById("code").value
    var stdin = document.getElementById("stdin").value

    var ecode = Base64.encode(code)
    var estdin = Base64.encode(stdin)

    var xmlhttp;
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("POST", "https://dev.covariant.cn/cgi/cs-online-wxacode", true);
    xmlhttp.responseType = "blob";
    xmlhttp.onload = function () {
        console.log(this);
        if (this.status == 200) {
            var modal = document.getElementById('myModal');
            var modalImg = document.getElementById("wxacode");
            var captionText = document.getElementById("caption");
            modal.style.display = "block";
            modalImg.src = window.URL.createObjectURL(this.response);
            captionText.innerHTML = "使用微信扫码继续(只可扫一次)";
            var span = document.getElementsByClassName("close")[0];
            span.onclick = function () {
                modal.style.display = "none";
            }
        }
    }
    xmlhttp.send(JSON.stringify({ code: ecode, stdin: estdin }))
}