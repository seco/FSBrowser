// 
// 
// 

#define DBG_OUTPUT_PORT Serial

//#define DEBUG_DYNAMICDATA

#include "DynamicData.h"
#include "Config.h"
#include "FSWebServer.h"
#include <NtpClientLib.h>
#include <StreamString.h>

extern ntpClient* ntp;

#ifdef DEBUG_DYNAMICDATA
int wsNumber = 0;
#endif // DEBUG_DYNAMICDATA

const char Page_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="10; URL=/config.html">
Please Wait....Configuring and Restarting.
)=====";

const char Page_Restart[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="10; URL=/general.html">
Please Wait....Configuring and Restarting.
)=====";

void send_general_configuration_values_html()
{
	String values = "";
	values += "devicename|" + (String)config.DeviceName + "|input\n";
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

//
//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
//

void send_connection_state_values_html()
{

	String state = "N/A";
	String Networks = "";
	if (WiFi.status() == 0) state = "Idle";
	else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
	else if (WiFi.status() == 2) state = "SCAN COMPLETED";
	else if (WiFi.status() == 3) state = "CONNECTED";
	else if (WiFi.status() == 4) state = "CONNECT FAILED";
	else if (WiFi.status() == 5) state = "CONNECTION LOST";
	else if (WiFi.status() == 6) state = "DISCONNECTED";



	int n = WiFi.scanNetworks();

	if (n == 0)
	{
		Networks = "<font color='#FF0000'>No networks found!</font>";
	}
	else
	{


		Networks = "Found " + String(n) + " Networks<br>";
		Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
		Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
		for (int i = 0; i < n; ++i)
		{
			int quality = 0;
			if (WiFi.RSSI(i) <= -100)
			{
				quality = 0;
			}
			else if (WiFi.RSSI(i) >= -50)
			{
				quality = 100;
			}
			else
			{
				quality = 2 * (WiFi.RSSI(i) + 100);
			}


			Networks += "<tr><td><a href='javascript:selssid(\"" + String(WiFi.SSID(i)) + "\")'>" + String(WiFi.SSID(i)) + "</a></td><td>" + String(quality) + "%</td><td>" + String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*") + "</td></tr>";
		}
		Networks += "</table>";
	}

	String values = "";
	values += "connectionstate|" + state + "|div\n";
	values += "networks|" + Networks + "|div\n";
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

//
//   FILL THE PAGE WITH VALUES
//

void send_network_configuration_values_html()
{

	String values = "";

	values += "ssid|" + (String)config.ssid + "|input\n";
	values += "password|" + (String)config.password + "|input\n";
	values += "ip_0|" + (String)config.IP[0] + "|input\n";
	values += "ip_1|" + (String)config.IP[1] + "|input\n";
	values += "ip_2|" + (String)config.IP[2] + "|input\n";
	values += "ip_3|" + (String)config.IP[3] + "|input\n";
	values += "nm_0|" + (String)config.Netmask[0] + "|input\n";
	values += "nm_1|" + (String)config.Netmask[1] + "|input\n";
	values += "nm_2|" + (String)config.Netmask[2] + "|input\n";
	values += "nm_3|" + (String)config.Netmask[3] + "|input\n";
	values += "gw_0|" + (String)config.Gateway[0] + "|input\n";
	values += "gw_1|" + (String)config.Gateway[1] + "|input\n";
	values += "gw_2|" + (String)config.Gateway[2] + "|input\n";
	values += "gw_3|" + (String)config.Gateway[3] + "|input\n";
	values += "dns_0|" + (String)config.DNS[0] + "|input\n";
	values += "dns_1|" + (String)config.DNS[1] + "|input\n";
	values += "dns_2|" + (String)config.DNS[2] + "|input\n";
	values += "dns_3|" + (String)config.DNS[3] + "|input\n";
	values += "dhcp|" + (String)(config.dhcp ? "checked" : "") + "|chk\n";
	
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

//
// FILL WITH INFOMATION
// 

void send_information_values_html()
{

	String values = "";

	values += "x_ssid|" + (String)WiFi.SSID() + "|div\n";
	values += "x_ip|" + (String)WiFi.localIP()[0] + "." + (String)WiFi.localIP()[1] + "." + (String)WiFi.localIP()[2] + "." + (String)WiFi.localIP()[3] + "|div\n";
	values += "x_gateway|" + (String)WiFi.gatewayIP()[0] + "." + (String)WiFi.gatewayIP()[1] + "." + (String)WiFi.gatewayIP()[2] + "." + (String)WiFi.gatewayIP()[3] + "|div\n";
	values += "x_netmask|" + (String)WiFi.subnetMask()[0] + "." + (String)WiFi.subnetMask()[1] + "." + (String)WiFi.subnetMask()[2] + "." + (String)WiFi.subnetMask()[3] + "|div\n";
	values += "x_mac|" + GetMacAddress() + "|div\n";
	values += "x_dns|" + (String)WiFi.dnsIP()[0] + "." + (String)WiFi.dnsIP()[1] + "." + (String)WiFi.dnsIP()[2] + "." + (String)WiFi.dnsIP()[3] + "|div\n";
	values += "x_ntp_sync|" + ntp->getTimeString(ntp->getLastNTPSync()) + "|div\n";
	values += "x_ntp_time|" + ntp->getTimeStr() + "|div\n";
	values += "x_ntp_date|" + ntp->getDateStr() + "|div\n";
	
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

String GetMacAddress()
{
	uint8_t mac[6];
	char macStr[18] = { 0 };
	WiFi.macAddress(mac);
	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return  String(macStr);
}

void send_NTP_configuration_values_html()
{

	String values = "";
	values += "ntpserver|" + (String)config.ntpServerName + "|input\n";
	values += "update|" + (String)config.Update_Time_Via_NTP_Every + "|input\n";
	values += "tz|" + (String)config.timezone + "|input\n";
	values += "dst|" + (String)(config.daylight ? "checked" : "") + "|chk\n";
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

void send_network_configuration_html()
{
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
	if (server.args() > 0)  // Save Settings
	{
		//String temp = "";
		bool oldDHCP = config.dhcp; // Save status to avoid general.html cleares it
		config.dhcp = false;
		for (uint8_t i = 0; i < server.args(); i++) {
#ifdef DEBUG_DYNAMICDATA
			DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, server.arg(i).c_str());
#endif // DEBUG_DYNAMICDATA
			if (server.argName(i) == "devicename") { 
				config.DeviceName = urldecode(server.arg(i));	
				config.dhcp = oldDHCP;
				continue; }
			if (server.argName(i) == "ssid") { config.ssid = urldecode(server.arg(i));	continue; }
			if (server.argName(i) == "password") {	config.password = urldecode(server.arg(i)); continue; }
			if (server.argName(i) == "ip_0") {	if (checkRange(server.arg(i))) 	config.IP[0] = server.arg(i).toInt(); continue;	}
			if (server.argName(i) == "ip_1") {  if (checkRange(server.arg(i))) 	config.IP[1] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "ip_2") {  if (checkRange(server.arg(i))) 	config.IP[2] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "ip_3") {  if (checkRange(server.arg(i))) 	config.IP[3] = server.arg(i).toInt(); continue;	}
			if (server.argName(i) == "nm_0") {  if (checkRange(server.arg(i))) 	config.Netmask[0] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "nm_1") {  if (checkRange(server.arg(i))) 	config.Netmask[1] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "nm_2") {  if (checkRange(server.arg(i))) 	config.Netmask[2] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "nm_3") {  if (checkRange(server.arg(i))) 	config.Netmask[3] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "gw_0") {  if (checkRange(server.arg(i))) 	config.Gateway[0] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "gw_1") {  if (checkRange(server.arg(i))) 	config.Gateway[1] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "gw_2") {  if (checkRange(server.arg(i))) 	config.Gateway[2] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "gw_3") {  if (checkRange(server.arg(i))) 	config.Gateway[3] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "dns_0") { if (checkRange(server.arg(i))) 	config.DNS[0] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "dns_1") { if (checkRange(server.arg(i))) 	config.DNS[1] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "dns_2") { if (checkRange(server.arg(i))) 	config.DNS[2] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "dns_3") { if (checkRange(server.arg(i))) 	config.DNS[3] = server.arg(i).toInt(); continue; }
			if (server.argName(i) == "dhcp") { config.dhcp = true; continue; }
		}
		server.send(200, "text/html", Page_WaitAndReload);
		save_config();
		ESP.restart();
		//ConfigureWifi();
		//AdminTimeOutCounter = 0;
	}
	else
	{
		DBG_OUTPUT_PORT.println(server.uri());
		handleFileRead(server.uri());
	}
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void send_general_configuration_html()
{
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
	if (server.args() > 0)  // Save Settings
	{
		for (uint8_t i = 0; i < server.args(); i++) {
#ifdef DEBUG_DYNAMICDATA
			DBG_OUTPUT_PORT.printf("Arg %d: %s\n", i, server.arg(i).c_str());
#endif // DEBUG_DYNAMICDATA
			if (server.argName(i) == "devicename") {
				config.DeviceName = urldecode(server.arg(i)); 
				continue;
			}
		}
		server.send(200, "text/html", Page_Restart);
		save_config();
		ESP.restart();
		//ConfigureWifi();
		//AdminTimeOutCounter = 0;
	}
	else
	{
		handleFileRead(server.uri());
	}
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void send_NTP_configuration_html()
{
	if (server.args() > 0)  // Save Settings
	{
		config.daylight = false;
		//String temp = "";
		for (uint8_t i = 0; i < server.args(); i++) {
			if (server.argName(i) == "ntpserver") {
				config.ntpServerName = urldecode(server.arg(i));
				ntp->setNtpServerName(config.ntpServerName);
				continue;
			}
			if (server.argName(i) == "update") {
				config.Update_Time_Via_NTP_Every = server.arg(i).toInt();
				ntp->setInterval(config.Update_Time_Via_NTP_Every * 60);
				continue;
			}
			if (server.argName(i) == "tz") {
				config.timezone = server.arg(i).toInt();
				ntp->setTimeZone(config.timezone / 10);
				continue;
			}
			if (server.argName(i) == "dst") {
				config.daylight = true;
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("Daylight Saving: %d\n", config.daylight);
#endif // DEBUG_DYNAMICDATA
				continue;
			}
		}
		
		ntp->setDayLight(config.daylight);
		save_config();
		//firstStart = true;
		
		setTime(ntp->getTime()); //set time
	}
	handleFileRead("/ntp.html");
	//server.send(200, "text/html", PAGE_NTPConfiguration);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

void restart_esp() {
	//server.send(200, "text/html", Page_Restart);
	delay(1000);
	ESP.reset();
}

void send_wwwauth_configuration_values_html() {
	String values = "";

	values += "wwwauth|" + (String)(httpAuth.auth ? "checked" : "") + "|chk\n";
	values += "wwwuser|" + (String)httpAuth.wwwUsername + "|input\n";
	values += "wwwpass|" + (String)httpAuth.wwwPassword + "|input\n";
	
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void send_wwwauth_configuration_html()
{
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.printf("%s %d\n",__FUNCTION__, server.args());
#endif // DEBUG_DYNAMICDATA
	if (server.args() > 0)  // Save Settings
	{
		httpAuth.auth = false;
		//String temp = "";
		for (uint8_t i = 0; i < server.args(); i++) {
			if (server.argName(i) == "wwwuser") {
				httpAuth.wwwUsername = urldecode(server.arg(i));
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("User: %s\n", httpAuth.wwwUsername.c_str());
#endif // DEBUG_DYNAMICDATA
				continue;
			}
			if (server.argName(i) == "wwwpass") {
				httpAuth.wwwPassword = urldecode(server.arg(i));
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("Pass: %s\n", httpAuth.wwwPassword.c_str());
#endif // DEBUG_DYNAMICDATA
				continue;
			}
			if (server.argName(i) == "wwwauth") {
				httpAuth.auth = true;
#ifdef DEBUG_DYNAMICDATA
				DBG_OUTPUT_PORT.printf("HTTP Auth enabled\n");
#endif // DEBUG_DYNAMICDATA
				continue;
			}
		}

		saveHTTPAuth();
	}
	handleFileRead("/system.html");
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA

}

void send_update_firmware_values_html() {
	String values = "";
	uint32_t maxSketchSpace = (ESP.getSketchSize() - 0x1000) & 0xFFFFF000;
	//bool updateOK = Update.begin(maxSketchSpace);
	bool updateOK = maxSketchSpace < ESP.getFreeSketchSpace();
	StreamString result;
	Update.printError(result);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.printf("--MaxSketchSpace: %d\n", maxSketchSpace);
	DBG_OUTPUT_PORT.printf("--Update error = %s\n", result.c_str());
#endif // DEBUG_DYNAMICDATA
	values += "remupd|" + (String)((updateOK) ? "OK" : "ERROR") + "|div\n";
	
	if (Update.hasError()) {
		result.trim();
		values += "remupdResult|" + result + "|div\n";
	}
	else {
		values += "remupdResult||div\n";
	}
	
	server.send(200, "text/plain", values);
#ifdef DEBUG_DYNAMICDATA
	DBG_OUTPUT_PORT.println(__FUNCTION__);
#endif // DEBUG_DYNAMICDATA
}

void sendTimeData() {
	for (int i = 0; i < WEBSOCKETS_SERVER_CLIENT_MAX; i++) {
#ifdef DEBUG_DYNAMICDATA
		//DBG_OUTPUT_PORT.println(__PRETTY_FUNCTION__);
#endif // DEBUG_DYNAMICDATA
		String time = "T" + ntp->getTimeStr();
		wsServer.sendTXT(i, time);
		String date = "D" + ntp->getDateStr();
		wsServer.sendTXT(i, date);
		String sync = "S" + ntp->getTimeString(ntp->getLastNTPSync());
		wsServer.sendTXT(i, sync);
	}
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

	switch (type) {
	case WStype_DISCONNECTED:
#ifdef DEBUG_DYNAMICDATA
		DBG_OUTPUT_PORT.printf("[%u] Disconnected!\n", num);
#endif // DEBUG_DYNAMICDATA
		break;
	case WStype_CONNECTED:
	{
#ifdef DEBUG_DYNAMICDATA
		wsNumber = num;
		IPAddress ip = wsServer.remoteIP(num);
		DBG_OUTPUT_PORT.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif // DEBUG_DYNAMICDATA

		// send message to client
		//wsServer.sendTXT(num, "Connected");
	}
	break;
	case WStype_TEXT:
#ifdef DEBUG_DYNAMICDATA
		DBG_OUTPUT_PORT.printf("[%u] get Text: %s\n", num, payload);
#endif // DEBUG_DYNAMICDATA
		// send message to client
		// webSocket.sendTXT(num, "message here");

		// send data to all connected clients
		// webSocket.broadcastTXT("message here");
		break;
	case WStype_BIN:
#ifdef DEBUG_DYNAMICDATA
		DBG_OUTPUT_PORT.printf("[%u] get binary lenght: %u\n", num, lenght);
#endif // DEBUG_DYNAMICDATA
		hexdump(payload, lenght);

		// send message to client
		// webSocket.sendBIN(num, payload, lenght);
		break;
	}

}
