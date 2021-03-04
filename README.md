# CovScript Online Interpreter

Try it on: http://dev.covariant.cn/ or WeChat Miniprogram

![](html/wechat.jpg)

## Online Interpreter Web API(HTTP POST)
POST following contents to server, and it will return the contents with stdout:
```
{
    "code": "<base64-of-code>",
    "stdin": "<base64-of-stdin>",
}
```
Our Public Server:
```
http://dev.covariant.cn/cgi/cs-online (unencrypted)
https://dev.covariant.cn/cgi/cs-online (encrypted)
```
## Run CovScript Code with `ajax`
```js
postData = {
    code: window.btoa("<code-here>"), 
    stdin: window.btoa("<stdin-here>")
}

$.ajax({
    type: "post",
    url: "http://dev.covariant.cn/cgi/cs-online",
    data: JSON.stringify(postData),
    async: true,
    success: function (data) {
        // Your code here
    },
    error: function (data) {
        // Your code here
    }
})
```