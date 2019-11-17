CovScript Online IDE
====================

Enjoy coding!!!!

# Online IDE API
* Run CovScript Code with customized stdin input
    ```
    POST http://dev.covariant.cn/cgi/cs-online
    
    {
        "code": "<base64-of-code>",
        "stdin": "<base64-of-stdin>",
    }
    ```

* Run CovScript Code with `ajax`
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
    });
    ```