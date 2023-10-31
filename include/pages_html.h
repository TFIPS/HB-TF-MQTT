const char hbtfmqtt_css[] PROGMEM = R"=====(
.overlayPrimary 
{
    opacity:	0.7; 
    background: #000000;
    color:		#FFFFFF;
    width:      100%;
    height:     100%; 
    z-index:    999;
    top:        0; 
    left:       0; 
    position:   fixed;
}
)=====";

const char index_html[] PROGMEM = R"=====(
	<!doctype html>
	<html lang='de'>
		<head>
			<meta charset='utf-8'>
			<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>
			<link href="/hbtfmqtt.css" rel="stylesheet">
			<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/css/bootstrap.min.css" rel="stylesheet" crossorigin="anonymous">
			<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/js/bootstrap.bundle.min.js" crossorigin="anonymous"></script>
			<script src="https://code.jquery.com/jquery-3.6.0.min.js" integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=" crossorigin="anonymous"></script>
			<title>hbTFmqtt</title>
		</head>
		<body id="body">
			<!-- STARTMENU start -->
			<nav class="navbar navbar-expand-lg bg-light">
				<div class="container-fluid">
					<a class="navbar-brand" href="#">TFbleGW</a>
					<button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNavAltMarkup" aria-controls="navbarNavAltMarkup" aria-expanded="false" aria-label="Toggle navigation">
						<span class="navbar-toggler-icon"></span>
					</button>
					<div class="collapse navbar-collapse" id="navbarNavAltMarkup">
						<div class="navbar-nav">
							<a class="nav-link active" href="index">Startseite</a>
							<a class="nav-link" href="settings">Einstellungen</a>
						</div>
					</div>
				</div>
			</nav>
			<!-- STARTMENU end -->
			<div class="container-fluid">
				<div class="m-3">
					<div class="row my-2">
						<h1>Startseite</h1>
					</div>
					<div id="messageCard" class="card mb-3 d-none">
						<div class="card-body p-4">
							<h2>Information</h2>
							<div class="row mt-4">
								<div id="message" class="form-outline mt-2">-</div>
							</div>
						</div>
					</div>
					<div class="card">
						<div id="infoCard" class="card-body p-4">
							<h2>Geräteinformationen</h2>
							<div class="row mt-4">
								<div id="deviceIdLabel" class="col-xs-1 fw-bold">Device-ID:</div><div id="deviceId" class="col-11 fst-italic">-</div>
								<div id="deviceStateLabel" class="col-xs-1 fw-bold">Status:</div><div id="state" class="col-11">-</div>
								<div id="fVersionLabel" class="col-xs-1 fw-bold">Firmware:</div><div id="fVersion" class="col-11 fst-italic">-</div>
							</div>
						</div>
					</div>
				</div>
			</div>
			<!---
				<div class='d-block d-sm-none' data-device='xs'>xs</div>
				<div class='d-none d-sm-block d-md-none' data-device='sm'>sm</div>
				<div class='d-none d-md-block d-lg-none' data-device='md'>md</div>
				<div class='d-none d-lg-block d-xl-none' data-device='lg'>lg</div>
				<div class='d-none d-xl-block' data-device='xl'>xl</div>
			--!>
			<script>
				function start()
				{
					const data					= { "dataType": "indexGet" };
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
				}

				function wsData(event) 
				{
					var jData = JSON.parse(event.data);
					console.log(event.data)
					if(jData.hasOwnProperty("dataType") && jData.hasOwnProperty("data"))
					{
						switch(jData["dataType"])
						{
							case "indexGet" :
								if(jData["data"].hasOwnProperty("deviceId"))
								{			
									$("#deviceId").text(jData["data"]["deviceId"]);
								}
								if(jData["data"].hasOwnProperty("state"))
								{
									switch(jData["data"]["state"])
									{
										case 'fVersion'		: $("#state").text("Firmware muss aktualisiert werden");break;
										case 'ban'			: $("#state").text("Gerät gesperrt");break;
										case 'offline' 		: $("#state").text("Gerät ist offline");break;
										case 'connected' 	: $("#state").text("Gerät hat eine Verbindung zur Cloud");break;
										case 'waiting' 		: $("#state").text("Gerät wartet auf Einstellungen");break;
										case 'active' 		: $("#state").text("Gerät arbeitet");break;
									}
								}
								if(jData["data"].hasOwnProperty("fVersion"))
								{			
									$("#fVersion").text(jData["data"]["fVersion"]);
								}
								if(jData["data"].hasOwnProperty("message"))
								{
									$("#messageCard").removeClass("d-none")
									$("#message").val(jData["data"]["message"]);
								}
							break;
						}
					}
				}

				// DEFAULT FUNCTIONS
				let websocket;
				var gateway 				= `ws://${window.location.hostname}/ws`;

				webSocketStart();

				function wsOpen()
				{
					overlayRemove('wsLost');
					if (typeof start === "function") 
					{  
						start();
					}
				}
				function wsClose()
				{
					overlayAdd('primary', true, 'wsLost', 'Stelle Verbindung wieder her...');
					setTimeout(webSocketStart, 5000);
				}
				function overlayAdd(css, wait, name, text)
				{
					css = css[0].toUpperCase() + css.slice(1);
					var overlayDiv = document.getElementById("overlayDiv");
					if(!overlayDiv)
					{
						var e = document.createElement('div');
						e.id		= 'overlayDiv'
						e.className = 'overlay'+css+' d-flex justify-content-center align-items-center';
						e.innerHTML = '<div id="overlaySpinner" class="spinner-border" style="width: 3rem; height: 3rem;"></div>\
							<span id="overlayText" name="'+name+'" class="p-3">'+text+'</span>';
						document.body.prepend(e);
					}
					else
					{
						overlayDiv.className = 'overlay'+css+' d-flex justify-content-center align-items-center';
						overlayDiv.innerHTML = '<div id="overlaySpinner" class="spinner-border" style="width: 3rem; height: 3rem;"></div>\
							<span id="overlayText" name="'+name+'" class="p-3">'+text+'</span>';
					}
					if(!wait)
					{
						var spinner = document.getElementById("overlaySpinner");
						if(spinner)
						{
							spinner.remove();
						}
					}
				}
				function overlayRemove(name)
				{
					var overlayDiv = document.getElementById("overlayDiv");
					if(overlayDiv)
					{
						var overlayText = document.getElementById("overlayText");
						if(overlayText.getAttribute("name") == name)
						{
							overlayDiv.remove();
						}
					}
				}
				function webSocketStart()
				{
					websocket 	        	= new WebSocket(gateway);
					if (typeof wsOpen === "function") 
					{ 
						websocket.onopen    = wsOpen;
					}
					if (typeof wsClose === "function")
					{
						websocket.onclose   = wsClose;
					}
					if (typeof wsData === "function")
					{
						websocket.onmessage = wsData;
					}
					return websocket;
				}
			</script>
		</body>
	</html>
)=====";

const char settingsSimple_html[] PROGMEM = R"=====(
	<!doctype html>
	<html lang='de'>
		<head>
			<meta charset='utf-8'>
			<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>
			<title>hbTFmqtt</title>
		</head>
		<body>
			<h2>WLAN</h2>
			<table>
				<tbody>
				<tr>
					<th><button id="wlanScanBtn" onclick="wlanScan()">WLAN scannen</button></th>
					<th>
						<select name="wlanSelect" id="wlanSelect" style="width: 100%;" onChange="wlanNameSet(this)" disabled>
							<option value="">Bitte scannen</option>
						</select>
					</th>
				</tr>
				<tr>
					<td><label for="wlanName">SSID:</label></td>
					<td><input type="text" id="wlanName" class="form-control" onChange="dataCheck()"></td>
				</tr>
				<tr>
					<td><label for="wlanPassword">Passwort:</label></td>
					<td><input type="password" id="wlanPassword" class="form-control" onChange="dataCheck()"></td>
				</tr>
				<tr>
					<td><button id="wlanConnectBtn" class="btn btn-secondary" onclick="wlanConnect()" disabled>WLAN verbinden</button></td>
					<td>&nbsp;</td>
				</tr>
				</tbody>
			</table>
			<script>
				function wlanScan()
				{
					var wlanScanBtn				= document.getElementById('wlanScanBtn');
					wlanScanBtn.disabled		= true;
					const data					= { "dataType": "wlanScan" };
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
					setTimeout(function() 
					{
						wlanScanBtn.disabled	= false;
					}, 10000);
				}
				function wsData(event) 
				{
					var jData = JSON.parse(event.data);
					
					if(jData.hasOwnProperty("dataType") && jData.hasOwnProperty("data"))
					{
						switch(jData["dataType"])
						{
							case "wlanStations" :
								var wlanScanBtn			= document.getElementById('wlanScanBtn');
								wlanScanBtn.disabled	= false;
								var wlanSelect			= document.getElementById('wlanSelect');
								wlanSelect.length 		= 0;
								if(jData["data"] == "none")
								{
									var option 			= document.createElement("option");
									option.value 		= "";
									option.text 		= "Kein WLAN gefunden";
									wlans.appendChild(option);
								}
								else
								{
									wlanSelect.disabled	= false;
									var option 			= document.createElement("option");
									option.value 		= "";
									option.text 		= "WLAN auswählen";
									wlanSelect.appendChild(option);
									for (const [key, value] of Object.entries(jData["data"])) 
									{
										var option 		= document.createElement("option");
										option.value 	= key;
										option.text 	= 'SSID: '+key+' - Qualität: '+Math.min(Math.max(2 * (parseInt(value["rssi"]) + 100), 0), 100)+'% - Kanal: '+value["channel"];
										wlanSelect.appendChild(option);
									}
								}
							break;
						}
					}
				}
				function wlanNameSet(selectObject)
				{
					var wlanName 						= document.getElementById('wlanName');
					wlanName.value						= selectObject.value;
				}
				function wlanConnect()
				{
					var wlanName 						= document.getElementById('wlanName');
					var wlanPassword					= document.getElementById('wlanPassword');
					const data							= { "dataType": "wlanConnect", "data": {"wlanName": wlanName.value, "wlanPassword": wlanPassword.value}};
					dataJson 							= JSON.stringify(data);
					websocket.send(dataJson);
				}
				function dataCheck()
				{
					var wlanName						= document.getElementById('wlanName');
					var wlanPassword					= document.getElementById('wlanPassword');
					var wlanConnectBtn					= document.getElementById('wlanConnectBtn');

					if(wlanName.value.length > 0 && wlanPassword.value.length > 0)
					{
						wlanConnectBtn.disabled	= false;
					}
					else
					{
						wlanConnectBtn.disabled	= true;
					}
				}
				var gateway = `ws://${window.location.hostname}/ws`;
				var websocket;

				webSocketStart();
			
				function webSocketStart()
				{
					websocket 	        = new WebSocket(gateway);
					if (typeof wsOpen === "function") 
					{ 
						websocket.onopen    = wsOpen;
					}
					if (typeof wsClose === "function")
					{
						websocket.onclose   = wsClose;
					}
					if (typeof wsData === "function")
					{
						websocket.onmessage = wsData;
					}
				}
			</script>
		</body>
	</html>
)=====";

const char settings_html[] PROGMEM = R"=====(
	<!doctype html>
	<html lang='de'>
		<head>
			<meta charset='utf-8'>
			<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>
			<link href="/hbtfmqtt.css" rel="stylesheet">
			<link href="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/css/bootstrap.min.css" rel="stylesheet" crossorigin="anonymous">
			<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.2.2/dist/js/bootstrap.bundle.min.js" crossorigin="anonymous"></script>
			<script src="https://code.jquery.com/jquery-3.6.0.min.js" integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=" crossorigin="anonymous"></script>
			<title>hbTFmqtt</title>
		</head>
		<body id="body">
			<!-- STARTMENU start -->
			<nav class="navbar navbar-expand-lg bg-light">
				<div class="container-fluid">
					<a class="navbar-brand" href="#">hbTFmqtt</a>
					<button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNavAltMarkup" aria-controls="navbarNavAltMarkup" aria-expanded="false" aria-label="Toggle navigation">
						<span class="navbar-toggler-icon"></span>
					</button>
					<div class="collapse navbar-collapse" id="navbarNavAltMarkup">
						<div class="navbar-nav">
							<a class="nav-link" href="index">Startseite</a>
							<a class="nav-link active" href="settings">Einstellungen</a>
						</div>
					</div>
				</div>
			</nav>
			<!-- STARTMENU end -->
			<!-- MODALS start -->
			<div id="fUpdateModal" class="modal" tabindex="-1">
				<div class="modal-dialog">
					<div class="modal-content">
						<div class="modal-header">
							<h5 class="modal-title">Firmware-Update</h5>
							<button type="button" class="btn-close" data-bs-dismiss="modal"></button>
						</div>
						<div class="modal-body">
							<p>Soll das Firmware-Update wirklich durchgeführt werden?</p>
						</div>
                            <div class="modal-footer">
                                <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Schließen</button>
							    <button type="button" class="btn btn-success" onclick="fUpdate('beta')">Beta Firmware Update</button>
                            </div>
						</div>
					</div>
				</div>
			</div>
			<div id="rebootModal" class="modal" tabindex="-1">
				<div class="modal-dialog">
					<div class="modal-content">
						<div class="modal-header">
							<h5 class="modal-title">Neustart</h5>
							<button type="button" class="btn-close" data-bs-dismiss="modal"></button>
						</div>
						<div class="modal-body">
							<p>Soll der Neustart wirklich durchgeführt werden?</p>
						</div>
						<div class="modal-footer">
							<button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Schließen</button>
							<button type="button" class="btn btn-primary" onclick="reboot()">Neustart durchführen</button>
						</div>
					</div>
				</div>
			</div>
			<!-- MODALS end -->
			<div class="container-fluid">
				<div class="m-3">
					<div class="row my-2">
						<h1>Einstellungen</h1>
					</div>
					<div class="card mb-3">
						<div class="card-body p-4">
							<h2>WLAN</h2>
							<div class="row mt-4">
								<div class="col-md-2 mb-4">
									<button id="wlanScanBtn" name="wlanScanBtn" type="button" onclick="wlanScan()" class="btn btn-outline-primary w-100">WLAN scannen</button>
								</div>
								<div class="col-md-10 mb-4">
									<select id="wlanSelect" name="wlanSelect" onChange="wlanNameSet(this)" class="form-select" disabled>
										<option value="">Bitte scannen</option>
									</select>
								</div>
							</div>
							<div class="row">
								<div class="mb-4">
									<div class="form-outline">
										<label for="wlanName">SSID:</label>
										<input id="wlanName" name="wlanName" type="text" class="form-control" onChange="dataCheck()" value="">
									</div>
								</div>
							</div>
							<div class="row">
								<div class="mb-4">
									<div class="form-outline">
										<label for="wlanPassword">Passwort:</label>
										<input id="wlanPassword" name="wlanPassword" type="password" class="form-control" onChange="dataCheck()" value="">
									</div>
								</div>
							</div>
							<div class="row">
								<div class="mt-2">
									<div class="form-outline">
										<button id="wlanConnectBtn" name="wlanConnectBtn" type="button" class="btn btn-secondary" onclick="wlanConnect()" disabled>WLAN verbinden</button>
									</div>
								</div>
							</div>
						</div>
					</div>

					<form id="settingsIpForm" class="form-input">
						<div class="card mb-3">
							<div class="card-body p-4">
								<h2>Netzwerk</h2>
								<div class="row mt-4">
									<div class="mb-0">
										<div class="form-outline">
											<input name="action" type="hidden" value="settingsSet">
											<label for="ipAddressLabel">IP-Adresse:</label>
											<input id="ipAddress" name="ipAddress" type="text" class="form-control" value="">
										</div>
									</div>
								</div>
								<div class="row">
									<div class="mb-0">
										<div class="form-outline">
											<label for="subnetAddressLabel">Subnetz:</label>
											<input id="subnetAddress" name="subnetAddress" type="text" class="form-control" value="">
										</div>
									</div>
								</div>
								<div class="row">
									<div class="mb-0">
										<div class="form-outline">
											<div class="form-outline">
                                                <label for="gwAddressLabel">Gateway:</label>
                                                <input id="gwAddress" name="gwAddress" type="text" class="form-control" value="">
                                            </div>
										</div>
									</div>
								</div>
                                <div class="row">
									<div class="mb-5">
										<div class="form-outline">
											<div class="form-outline">
                                                <label for="dnsAddressLabel">DNS:</label>
                                                <input id="dnsAddress" name="dnsAddress" type="text" class="form-control" value="">
                                            </div>
										</div>
									</div>
								</div>
								<div class="row mt-4">
									<div class="mb-0">
										<button id="settingsSaveBtn" name="settingsSaveBtn" type="button" class="btn btn-success" onclick="verify('settingsIp')">Einstellungen speichern</button>
									</div>
								</div>
							</div>
						</div>
					</form>

					<form id="settingsMqttForm" class="form-input">
						<input name="action" type="hidden" value="settingsSet">
						<div class="card card-secondary">
							<div class="card-header">
								<h3 class="card-title">MQTT</h3>
							</div>
							<div class="card-body">
								<div class="row">
									<div class="col-sm-6">
										<div class="form-group">
											<label for="mqttHost">MQTT-Server</label>
											<input id="mqttHost" class="form-control" name="mqttHost" type="text">
										</div>
									</div>
									<div class="col-sm-6">
										<div class="form-group">
											<label for="mqttPort">MQTT-Port</label>
											<input id="mqttPort" class="form-control" name="mqttPort" type="text">
										</div>
									</div>
									<div class="form-group">
										<label for="mqttTopic">MQTT-Topic</label>
										<input id="mqttTopic" class="form-control" name="mqttTopic" type="text">
									</div>
									<div class="col-sm-6">
										<div class="form-group">
											<label for="mqttUsername">Username</label>
											<input id="mqttUsername" class="form-control" name="mqttUsername" type="text">
										</div>
									</div>
									<div class="col-sm-6">
										<div class="form-group">
											<label for="mqttPassword">Passwort</label>
											<input id="mqttPassword" class="form-control" name="mqttPassword" type="password" value="">
										</div>
									</div>
								</div>
							</div>
							<div class="card-footer">
								<button id="settingsSaveBtn" name="settingsSaveBtn" type="button" class="btn btn-success" onclick="verify('settingsMqtt')">Einstellungen speichern</button>
							</div>
						</div>
					</div>
				</div>
			</div>
			<script>
				function start()
				{
					const data					= { "dataType": "settingsGet" };
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
				}

				function wlanScan()
				{
					var wlanScanBtn				= document.getElementById('wlanScanBtn');
					wlanScanBtn.disabled		= true;
					const data					= { "dataType": "wlanScan" };
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
					setTimeout(function() 
					{
						wlanScanBtn.disabled	= false;
					}, 10000);
				}
				function wlanNameSet(selectObject)
				{
					var wlanName 				= document.getElementById('wlanName');
					wlanName.value				= selectObject.value;
				}
				function wlanConnect()
				{
					var wlanName 				= document.getElementById('wlanName');
					var wlanPassword			= document.getElementById('wlanPassword');
					const data					= { "dataType": "wlanConnect", "data": {"wlanName": wlanName.value, "wlanPassword": wlanPassword.value}};
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
				}

				function dataCheck()
				{
					var wlanName				= document.getElementById('wlanName');
					var wlanPassword			= document.getElementById('wlanPassword');
					var wlanConnectBtn			= document.getElementById('wlanConnectBtn');

					if(wlanName.value.length > 0 && wlanPassword.value.length > 0)
					{
						wlanConnectBtn.disabled	= false;
					}
					else
					{
						wlanConnectBtn.disabled	= true;
					}
				}

                function verify(id)
                {
                    error   = false;
                    switch(id)
                    {
                        case 'settingsIp':
                            if($('#ipAddress').val().length != 0 || $('#subnetAddress').val().length != 0 || $('#gwAddress').val().length != 0)
					        {
                                error = setIsValid('ipAddress', validateIP('ipAddress'), "Keine gültige IP-Adresse oder IP-Adresse unvollständig");
                            }
                            if($('#ipAddress').val().length != 0 || $('#subnetAddress').val().length != 0 || $('#gwAddress').val().length != 0)
                            {
                                error = setIsValid('subnetAddress', validateIP('subnetAddress'), "Kein gültiges Subnet oder Subnet unvollständig"); 
                            }
                            if($('#ipAddress').val().length != 0 || $('#subnetAddress').val().length != 0 || $('#gwAddress').val().length != 0)
                            {
                                error = setIsValid('gwAddress', validateIP('gwAddress'), "Keine gültige Gateway-Adresse oder Gateway-Adresse unvollständig"); 
                            }
                            if($('#dnsAddress').val().length != 0)
                            {
                                error = setIsValid('dnsAddress', validateIP('dnsAddress'), "Keine gültige DNS-Adresse oder DNS-Adresse unvollständig"); 
                            }
                        break;
                        case 'settingsWf':
                            if($('#webUsername').val().length != 0 || $('#webPassword').val().length != 0)
                            {
                                if($('#webUsername').val().length > 0 && $('#webPassword').val().length > 0)
					            {
                                    setIsValid('webUsername', true);
                                    setIsValid('webPassword', true);
                                }
                                else
                                {
                                    if($('#webUsername').val().length == 0)
					                {
                                        error = setIsValid('webUsername', false, "Bitte Usernamen ausfüllen");
                                    }
                                    else
                                    {
                                        setIsValid('webUsername', true);
                                    }
                                    if($('#webPassword').val().length == 0)
                                    {
                                        error = setIsValid('webPassword', false, "Bitte Passwort ausfüllen");
                                    }
                                    else
                                    {
                                        setIsValid('webPassword', true);
                                    }
                                }
                            }
                        break;
                        case 'settingsMqtt':
                            error = setIsValid('mqttHost', validateIP('mqttHost'), "Keine gültige IP-Adresse oder IP-Adresse unvollständig");
							error = setIsValid('mqttPort', validatePort('mqttPort'), "Keine gültiger Port");

                            if($('#mqttUsername').val().length > 0 && $('#mqttPassword').val().length > 0 && $('#mqttTopic').val().length > 0)
                            {
								setIsValid('mqttTopic', true);
                                setIsValid('mqttUsername', true);
                                setIsValid('mqttPassword', true);
                            }
                            else
                            {
                                if($('#mqttTopic').val().length > 0 || $('#mqttUsername').val().length > 0 || $('#mqttPassword').val().length > 0)
                                {
									if($('#mqttTopic').val().length == 0)
                                    {
                                        error = setIsValid('mqttTopic', false, "Bitte Topic angeben");
                                    }
                                    else
                                    {
                                        setIsValid('mqttTopic', true);
                                    }
                                    if($('#mqttUsername').val().length == 0)
                                    {
                                        error = setIsValid('mqttUsername', false, "Bitte Usernamen ausfüllen");
                                    }
                                    else
                                    {
                                        setIsValid('mqttUsername', true);
                                    }
                                    if($('#mqttPassword').val().length == 0)
                                    {
                                        error = setIsValid('mqttPassword', false, "Bitte Passwort ausfüllen");
                                    }
                                    else
                                    {
                                        setIsValid('mqttPassword', true);
                                    }
                                }
                            }
                        break;
                    }

                    if(!error)
                    {
                        var setForm	= document.getElementById(id+"Form");
					    setData 	= new FormData(setForm);
					    var setJson	= JSON.stringify(Object.fromEntries(setData.entries()));

					    const data	= '{"dataType": "'+id+'", "data": '+setJson+'}';
					    websocket.send(data);
                    }
                }

                function validateIP(id)
                {
					const octet = '(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]?|0)';
                    const regex = new RegExp(`^${octet}\\.${octet}\\.${octet}\\.${octet}$`);
                    return regex.test($('#'+id).val());
                }

				function validatePort(id)
                {
					const regex = /^([1-9]\d{0,3}|[1-5]\d{4}|6[0-4]\d{3}|65[0-4]\d{2}|655[0-2]\d|6553[0-5])$/;
  					return regex.test($('#'+id).val());
                }

				function setIsValid($id, $valid, $text="", $validClass="is-valid", $invalidClass="is-invalid")
				{
					switch($valid)
					{
						case false :
							if(!$('#help_'+$id).length)
							{
								$('#'+$id).after('<small id="help_'+$id+'" class="text-danger"></small>');
							}
							$('#help_'+$id).text($text);
							if($('#'+$id).hasClass($validClass))
							{
								$('#'+$id).removeClass($validClass);
							}
							$('#'+$id).addClass($invalidClass);
							return true;
						break;
						case true :
							if($('#help_'+$id).length)
							{
								$('#help_'+$id).remove();
							}
							if($('#'+$id).hasClass($invalidClass))
							{
								$('#'+$id).removeClass($invalidClass);
							}
							$('#'+$id).addClass($validClass);
							return false;
						break;
					}
				}

				function fUpdate(type)
				{
					$("#fUpdateModal").modal("hide");
					const data					= { "dataType": "fUpdate", "data": {"fType" : type}};
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
				}

				function reboot()
				{
					$("#rebootModal").modal("hide");
					overlayAdd('primary', true, 'reboot', 'Neustart wird durchgeführt...');
					
					const data					= { "dataType": "reboot" };
					dataJson 					= JSON.stringify(data);
					websocket.send(dataJson);
				}

				function showModal(name)
				{
					$(document).ready(function() {
						$("#"+name+"Modal").modal("show");
					});
				}

				function wsData(event) 
				{
					var jData = JSON.parse(event.data);
					
					if(jData.hasOwnProperty("dataType") && jData.hasOwnProperty("data"))
					{
						switch(jData["dataType"])
						{
							case "settingsGet" :								
								if(jData["data"].hasOwnProperty("wlanName"))
								{
									$("#wlanName").val(jData["data"]["wlanName"]);
								}
                                if(jData["data"].hasOwnProperty("ipAddress"))
								{
									$("#ipAddress").val(jData["data"]["ipAddress"]);
								}
                                if(jData["data"].hasOwnProperty("subnetAddress"))
								{
									$("#subnetAddress").val(jData["data"]["subnetAddress"]);
								}
                                if(jData["data"].hasOwnProperty("gwAddress"))
								{
									$("#gwAddress").val(jData["data"]["gwAddress"]);
								}
                                if(jData["data"].hasOwnProperty("dnsAddress"))
								{
									$("#dnsAddress").val(jData["data"]["dnsAddress"]);
								}
								if(jData["data"].hasOwnProperty("mqttHost"))
								{
									$("#mqttHost").val(jData["data"]["mqttHost"]);
								}
								if(jData["data"].hasOwnProperty("mqttPort"))
								{
									$("#mqttPort").val(jData["data"]["mqttPort"]);
								}
								if(jData["data"].hasOwnProperty("mqttTopic"))
								{
									$("#mqttTopic").val(jData["data"]["mqttTopic"]);
								}
								if(jData["data"].hasOwnProperty("mqttUsername"))
								{
									$("#mqttUsername").val(jData["data"]["mqttUsername"]);
								}
							break;
							// settingsGet fertig
							case "wlanStations" :
								var wlanScanBtn			= document.getElementById('wlanScanBtn');
								wlanScanBtn.disabled	= false;
								var wlanSelect			= document.getElementById('wlanSelect');
								wlanSelect.length 		= 0;
								if(jData["data"] == "none")
								{
									var option 			= document.createElement("option");
									option.value 		= "";
									option.text 		= "Kein WLAN gefunden";
									wlanSelect.appendChild(option);
								}
								else
								{
									wlanSelect.disabled	= false;
									wlanSelect.classList.add("bg-success");
									wlanSelect.classList.add("text-white");
									var option 			= document.createElement("option");
									option.value 		= "";
									option.text 		= "WLAN auswählen";
									wlanSelect.appendChild(option);
									for (const [key, value] of Object.entries(jData["data"])) 
									{
										var option 		= document.createElement("option");
										option.value 	= key;
										option.text 	= 'SSID: '+key+' - Qualität: '+Math.min(Math.max(2 * (parseInt(value["rssi"]) + 100), 0), 100)+'% - Kanal: '+value["channel"];
										wlanSelect.appendChild(option);
									}
								}
							break;
						}
					}
				}

				// DEFAULT FUNCTIONS
				let websocket;
				var gateway = `ws://${window.location.hostname}/ws`;

				webSocketStart();

				function wsOpen()
				{
					overlayRemove('wsLost');
					if (typeof start === "function") 
					{  
						start();
					}
				}
				function wsClose()
				{
					overlayAdd('primary', true, 'wsLost', 'Stelle Verbindung wieder her...');
					setTimeout(webSocketStart, 5000);
				}
				function overlayAdd(css, wait, name, text)
				{
					css = css[0].toUpperCase() + css.slice(1);
					var overlayDiv = document.getElementById("overlayDiv");
					if(!overlayDiv)
					{
						var e = document.createElement('div');
						e.id		= 'overlayDiv'
						e.className = 'overlay'+css+' d-flex justify-content-center align-items-center';
						e.innerHTML = '<div id="overlaySpinner" class="spinner-border" style="width: 3rem; height: 3rem;"></div>\
							<span id="overlayText" name="'+name+'" class="p-3">'+text+'</span>';
						document.body.prepend(e);
					}
					else
					{
						overlayDiv.className = 'overlay'+css+' d-flex justify-content-center align-items-center';
						overlayDiv.innerHTML = '<div id="overlaySpinner" class="spinner-border" style="width: 3rem; height: 3rem;"></div>\
							<span id="overlayText" name="'+name+'" class="p-3">'+text+'</span>';
					}
					if(!wait)
					{
						var spinner = document.getElementById("overlaySpinner");
						if(spinner)
						{
							spinner.remove();
						}
					}
				}
				function overlayRemove(name)
				{
					var overlayDiv = document.getElementById("overlayDiv");
					if(overlayDiv)
					{
						var overlayText = document.getElementById("overlayText");
						if(overlayText.getAttribute("name") == name)
						{
							overlayDiv.remove();
						}
					}
				}
				function webSocketStart()
				{
					websocket 	        	= new WebSocket(gateway);
					if (typeof wsOpen === "function") 
					{ 
						websocket.onopen    = wsOpen;
					}
					if (typeof wsClose === "function")
					{
						websocket.onclose   = wsClose;
					}
					if (typeof wsData === "function")
					{
						websocket.onmessage = wsData;
					}
					return websocket;
				}
			</script>
		</body>
	</html>
)=====";