function ajax_request(url, callback) {
    var xhr;
    if (typeof XMLHttpRequest !== "undefined") xhr = new XMLHttpRequest();
    else {
        var versions = ["MSXML2.XmlHttp.5.0", "MSXML2.XmlHttp.4.0", "MSXML2.XmlHttp.3.0", "MSXML2.XmlHttp.2.0", "Microsoft.XmlHttp"];
        for (var i = 0, len = versions.length; i < len; i++) {
            try {
                xhr = new ActiveXObject(versions[i]);
                break;
            } catch (e) {}
        }
    }
    xhr.onreadystatechange = ensureReadiness;

    function ensureReadiness() {
        if (xhr.readyState < 4) {
            return;
        }
        if (xhr.status !== 200) {
            return;
        }
        if (xhr.readyState === 4) {
            callback(xhr);
        }
    }
    xhr.open("GET", url, true);
    xhr.send("");
}

function on(element, event, callback) {
    if (element.addEventListener) {
        element.addEventListener(event, function() {
            callback(element, event);
        }, false);
    } else {
        if (element.attachEvent) {
            element.attachEvent(event, function() {
                callback(element, event);
            });
        }
    }
}

var inputs = document.getElementsByTagName("input");
for (var i = 0; i < inputs.length; i++) {
    if (inputs[i].type.toLowerCase() == "range") {
        ["change", "input"].forEach(function(ev) {
            on(inputs[i], ev, function(obj, ev) {
                if (ev == "change") {
                    ajax_request("/ledc?ch=" + obj.name.substr(4) + "&duty=" + obj.value, function() {
                        document.getElementById(obj.name).innerHTML = obj.value;
                    });
                }
                if (ev == "input") {
                    document.getElementById(obj.name).innerHTML = obj.value;
                }
            });
        });
    } else if ( inputs[i].type.toLowerCase() == "checkbox" ) {
		if ( inputs[i].getAttribute("rel") == "relay" ) {
			["change", "input"].forEach(function(ev) {
				on(inputs[i], ev, function(obj, ev) {
					if (ev == "change") {
						var vnew = 1 - parseInt(obj.getAttribute("value"));
						ajax_request("/gpio?pin=" + obj.name.substr(5) + "&st=" + vnew, function() {
							obj.setAttribute("value", vnew);			
							obj.checked = vnew;			
							document.getElementById(obj.name).innerHTML = vnew ? "ON" : "OFF";
						});
					}
					if (ev == "input") {
						document.getElementById(obj.name).innerHTML = obj.checked ? "ON" : "OFF";
					}
				});
			});			
		}
	}
}

var a = document.getElementsByTagName("a");
for (var i = 0; i < a.length; i++) {
    if (a[i].getAttribute("rel") == "relay") {
        on(a[i], "click", function(obj, ev) {
            var vnew = 1 - parseInt(obj.getAttribute("data-val"));
            ajax_request("/gpio?st=" + vnew + "&pin=" + obj.getAttribute("data-id"), function() {
                obj.setAttribute("data-val", vnew);
                obj.innerHTML = "<button class='relay " + (vnew ? "on" : "off") + "' >" + obj.getAttribute("data-title") + "</button>";
            });
        });
    }
}