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

function effects() {
	var eff = document.getElementById("effects");
	var effid = eff.options[eff.selectedIndex].value;
	var effname = eff.options[eff.selectedIndex].text;
	ajax_request("/colors?type=effect&id=" + effid, function() {
				eff.options[effid].selected="true";
				// var efft = document.getElementById("color");
                // efft.innerHTML = effname + "(" + effid + ")";
            });
}

function reboot() {
	ajax_request("/reboot?st=1", setTimeout( function() {window.location.replace("/");}, 3000));
	document.getElementById("rbt").style.display = "block";	
}

function i2cscan() {
	ajax_request("/i2cscan?st=1", function(res) {
		document.getElementById("i2cres").innerHTML = "<h4>Результаты сканирования:</h4><p>" + res.responseText + "</p>";	
	});
	
}

// id: id элемента, который нажали
// id2: id элемента, который надо изменять
// v:  подстановка результата запроса в innerHTML, 0 - не подставлять, 1 - добавлять к data-text, 2 - вставлять во внутрь data-text (перемемнная {0} )
// st: менять состояние кнопки, 1 - менять, 0 - не менять
function btnclick(id, id2, v, st) {
	var btn = document.getElementById( id );
	var uri = btn.getAttribute("data-uri");
	var value = btn.getAttribute("data-val");
	
	console.log('params: id = %s, id2 = %s, v = %d, st = %d', id, id2, v, st);
	
	ajax_request(uri + value, function(res) {
		
		if ( st == 1 ) {
			var vnew = 0;
			var resp = res.responseText
			if ( resp == "OFF" ) vnew = 0;
			else if ( resp == 0 ) vnew = 0;
			else if ( resp == "ON" ) vnew = 1;
			else if ( resp == 1 ) vnew = 1;
			else if ( resp == "ERROR" ) vnew = value;
			else if ( resp == "OK" ) vnew = !value;
			else vnew = 0;			
			var cls = btn.getAttribute("data-class");
			var snew = vnew > 0 ? " on" : " off";
			btn.setAttribute("class", cls + snew);
			btn.setAttribute("data-val", 1 - vnew); 
			
			console.log('resp = %s, vnew = %d, !vnew = %d', resp, vnew, 1 - vnew);
		}
		var elem = document.getElementById( id2 )
		if ( v == 1 ) {
			elem.innerHTML = btn.getAttribute("data-text") + res.responseText;	
		} else if ( v == 2 ) {
			var rs = btn.getAttribute("data-text");
			elem.innerHTML = rs.replace("{0}", res.responseText );	
		} else {
			elem.innerHTML = btn.getAttribute("data-text");	
		}
		
	});
}

function slider(val, name, uri) 
{
	console.log('params: val = %d, name = %s, uri = %s', val, name, uri);
	
	ajax_request(uri + val, function(res) {
		var resp = res.responseText;	
		var duty = document.getElementById( name );
		duty.innerHTML = resp; 		
	});

}