/** The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Modified to use Open-Meteo API (free, no API key required)
// https://open-meteo.com/

#include "OpenWeatherMapClient.h"

OpenWeatherMapClient::OpenWeatherMapClient(String city, String lat, String lon, boolean isMetric) {
  myCity = city;
  myLat = lat;
  myLon = lon;
  setMetric(isMetric);
}

void OpenWeatherMapClient::updateLocation(String city, String lat, String lon) {
  myCity = city;
  myLat = lat;
  myLon = lon;
}

void OpenWeatherMapClient::setMetric(boolean isMetric) {
  if (isMetric) {
    units = "celsius";
  } else {
    units = "fahrenheit";
  }
}

void OpenWeatherMapClient::setLanguage(String lang) {
  myLang = lang;
}

int OpenWeatherMapClient::getLangIndex() {
  if (myLang == "hu") return 1;
  if (myLang == "de") return 2;
  if (myLang == "fr") return 3;
  if (myLang == "es") return 4;
  return 0; // en
}

// Translation tables: [language][index]
// Languages: 0=en, 1=hu, 2=de, 3=fr, 4=es
// Conditions: 12 entries (short names for OLED display)
static const char* const condStrings[5][12] = {
  {"Clear", "Mostly Clear", "Partly Cloudy", "Overcast", "Fog", "Drizzle", "Rain", "Snow", "Showers", "Snow Showers", "Thunderstorm", "Unknown"},
  {"Derult", "Tobb. derult", "Reszben felhos", "Borult", "Kodos", "Szitalas", "Eso", "Havazas", "Zapor", "Hozapor", "Zivatar", "Ismeretlen"},
  {"Klar", "Uberw. klar", "Teilw. bewolkt", "Bedeckt", "Nebel", "Nieselregen", "Regen", "Schnee", "Schauer", "Schneeschauer", "Gewitter", "Unbekannt"},
  {"Degage", "Princ. degage", "Part. nuageux", "Couvert", "Brouillard", "Bruine", "Pluie", "Neige", "Averses", "Averses neige", "Orage", "Inconnu"},
  {"Despejado", "Mayor. despej.", "Parc. nublado", "Nublado", "Niebla", "Llovizna", "Lluvia", "Nieve", "Chubascos", "Chub. de nieve", "Tormenta", "Desconocido"}
};

// Descriptions: 29 entries (longer text for web display)
static const char* const descStrings[5][29] = {
  // English
  {"Clear sky", "Mainly clear", "Partly cloudy", "Overcast", "Fog", "Depositing rime fog",
   "Light drizzle", "Moderate drizzle", "Dense drizzle", "Light freezing drizzle", "Dense freezing drizzle",
   "Slight rain", "Moderate rain", "Heavy rain", "Light freezing rain", "Heavy freezing rain",
   "Slight snow fall", "Moderate snow fall", "Heavy snow fall", "Snow grains",
   "Slight rain showers", "Moderate rain showers", "Violent rain showers",
   "Slight snow showers", "Heavy snow showers",
   "Thunderstorm", "Thunderstorm with slight hail", "Thunderstorm with heavy hail", "Unknown"},
  // Hungarian
  {"Tiszta egbolt", "Tobbnyire derult", "Reszben felhos", "Borult", "Kod", "Zuzmaras kod",
   "Enyhe szitalas", "Mersekelt szitalas", "Suru szitalas", "Enyhe onos szitalas", "Suru onos szitalas",
   "Gyenge eso", "Mersekelt eso", "Eros eso", "Enyhe onos eso", "Eros onos eso",
   "Gyenge havazas", "Mersekelt havazas", "Eros havazas", "Hoszemcsek",
   "Gyenge zapor", "Mersekelt zapor", "Heves zapor",
   "Gyenge hozapor", "Eros hozapor",
   "Zivatar", "Zivatar gyenge jegesovel", "Zivatar eros jegesovel", "Ismeretlen"},
  // German
  {"Klarer Himmel", "Uberwiegend klar", "Teilweise bewolkt", "Bedeckt", "Nebel", "Reifnebel",
   "Leichter Nieselregen", "Massiger Nieselregen", "Starker Nieselregen", "Leichter gefr. Nieselregen", "Starker gefr. Nieselregen",
   "Leichter Regen", "Massiger Regen", "Starker Regen", "Leichter gefr. Regen", "Starker gefr. Regen",
   "Leichter Schneefall", "Massiger Schneefall", "Starker Schneefall", "Schneekorner",
   "Leichte Regenschauer", "Massige Regenschauer", "Heftige Regenschauer",
   "Leichte Schneeschauer", "Starke Schneeschauer",
   "Gewitter", "Gewitter mit leichtem Hagel", "Gewitter mit starkem Hagel", "Unbekannt"},
  // French
  {"Ciel degage", "Principalement degage", "Partiellement nuageux", "Couvert", "Brouillard", "Brouillard givrant",
   "Bruine legere", "Bruine moderee", "Bruine dense", "Bruine verglacante legere", "Bruine verglacante dense",
   "Pluie legere", "Pluie moderee", "Forte pluie", "Pluie verglacante legere", "Forte pluie verglacante",
   "Legeres chutes de neige", "Chutes de neige moderees", "Fortes chutes de neige", "Grains de neige",
   "Legeres averses", "Averses moderees", "Averses violentes",
   "Legeres averses de neige", "Fortes averses de neige",
   "Orage", "Orage avec grele legere", "Orage avec forte grele", "Inconnu"},
  // Spanish
  {"Cielo despejado", "Mayormente despejado", "Parcialmente nublado", "Nublado", "Niebla", "Niebla con escarcha",
   "Llovizna ligera", "Llovizna moderada", "Llovizna densa", "Llovizna helada ligera", "Llovizna helada densa",
   "Lluvia ligera", "Lluvia moderada", "Lluvia intensa", "Lluvia helada ligera", "Lluvia helada intensa",
   "Nevada ligera", "Nevada moderada", "Nevada intensa", "Granos de nieve",
   "Chubascos ligeros", "Chubascos moderados", "Chubascos violentos",
   "Chubascos de nieve ligeros", "Chubascos de nieve intensos",
   "Tormenta", "Tormenta con granizo ligero", "Tormenta con granizo intenso", "Desconocido"}
};

void OpenWeatherMapClient::updateWeather() {
  WiFiClient client;
  HTTPClient http;

  // Build Open-Meteo API URL
  String windUnit = "kmh";
  if (units == "fahrenheit") {
    windUnit = "mph";
  }
  String url = "http://api.open-meteo.com/v1/forecast?latitude=" + myLat + "&longitude=" + myLon
    + "&current=temperature_2m,relative_humidity_2m,weather_code,wind_speed_10m"
    + "&temperature_unit=" + units
    + "&wind_speed_unit=" + windUnit;

  Serial.println("Getting Weather Data from Open-Meteo");
  Serial.println(url);
  result = "";

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode != HTTP_CODE_OK) {
    Serial.println("HTTP error: " + String(httpCode));
    weathers[0].error = "HTTP Error: " + String(httpCode);
    http.end();
    return;
  }

  String payload = http.getString();
  http.end();

  Serial.println("Response length: " + String(payload.length()));

  const size_t bufferSize = 1024;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  weathers[0].cached = false;
  weathers[0].error = "";

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(payload);
  if (!root.success()) {
    Serial.println(F("Weather Data Parsing failed!"));
    Serial.println("Response: " + payload.substring(0, 200));
    weathers[0].error = "Weather Data Parsing failed!";
    return;
  }

  // Open-Meteo response structure:
  // { "current": { "time": "...", "temperature_2m": 5.9, "relative_humidity_2m": 97, "weather_code": 3, "wind_speed_10m": 10.1 } }

  if (!root.containsKey("current")) {
    Serial.println("Error: no current data in response");
    weathers[0].cached = true;
    weathers[0].error = "No current weather data";
    if (root.containsKey("reason")) {
      weathers[0].error = (const char*)root["reason"];
    }
    return;
  }

  float temp = root["current"]["temperature_2m"];
  int humidity = root["current"]["relative_humidity_2m"];
  int weatherCode = root["current"]["weather_code"];
  float windSpeed = root["current"]["wind_speed_10m"];

  weathers[0].lat = myLat;
  weathers[0].lon = myLon;
  weathers[0].dt = (const char*)root["current"]["time"];
  weathers[0].city = myCity;
  weathers[0].country = "";
  weathers[0].temp = String(temp);
  weathers[0].humidity = String(humidity);
  weathers[0].condition = wmoToCondition(weatherCode);
  weathers[0].wind = String(windSpeed);
  weathers[0].weatherId = String(weatherCode);
  weathers[0].description = wmoToDescription(weatherCode);
  weathers[0].icon = "";

  Serial.println("lat: " + weathers[0].lat);
  Serial.println("lon: " + weathers[0].lon);
  Serial.println("city: " + weathers[0].city);
  Serial.println("temp: " + weathers[0].temp);
  Serial.println("humidity: " + weathers[0].humidity);
  Serial.println("condition: " + weathers[0].condition);
  Serial.println("wind: " + weathers[0].wind);
  Serial.println("weatherCode: " + weathers[0].weatherId);
  Serial.println("description: " + weathers[0].description);
  Serial.println();
}

// Map WMO weather codes to condition index (0-11)
String OpenWeatherMapClient::wmoToCondition(int code) {
  int idx;
  if (code == 0) idx = 0;
  else if (code == 1) idx = 1;
  else if (code == 2) idx = 2;
  else if (code == 3) idx = 3;
  else if (code == 45 || code == 48) idx = 4;
  else if (code >= 51 && code <= 57) idx = 5;
  else if (code >= 61 && code <= 67) idx = 6;
  else if (code >= 71 && code <= 77) idx = 7;
  else if (code >= 80 && code <= 82) idx = 8;
  else if (code >= 85 && code <= 86) idx = 9;
  else if (code >= 95 && code <= 99) idx = 10;
  else idx = 11;
  return String(condStrings[getLangIndex()][idx]);
}

// Map WMO weather codes to description index (0-28)
String OpenWeatherMapClient::wmoToDescription(int code) {
  int idx;
  switch (code) {
    case 0: idx = 0; break;
    case 1: idx = 1; break;
    case 2: idx = 2; break;
    case 3: idx = 3; break;
    case 45: idx = 4; break;
    case 48: idx = 5; break;
    case 51: idx = 6; break;
    case 53: idx = 7; break;
    case 55: idx = 8; break;
    case 56: idx = 9; break;
    case 57: idx = 10; break;
    case 61: idx = 11; break;
    case 63: idx = 12; break;
    case 65: idx = 13; break;
    case 66: idx = 14; break;
    case 67: idx = 15; break;
    case 71: idx = 16; break;
    case 73: idx = 17; break;
    case 75: idx = 18; break;
    case 77: idx = 19; break;
    case 80: idx = 20; break;
    case 81: idx = 21; break;
    case 82: idx = 22; break;
    case 85: idx = 23; break;
    case 86: idx = 24; break;
    case 95: idx = 25; break;
    case 96: idx = 26; break;
    case 99: idx = 27; break;
    default: idx = 28; break;
  }
  return String(descStrings[getLangIndex()][idx]);
}

String OpenWeatherMapClient::roundValue(String value) {
  float f = value.toFloat();
  int rounded = (int)(f + 0.5f);
  return String(rounded);
}

String OpenWeatherMapClient::getWeatherResults() {
  return result;
}

String OpenWeatherMapClient::getLat(int index) {
  return weathers[index].lat;
}

String OpenWeatherMapClient::getLon(int index) {
  return weathers[index].lon;
}

String OpenWeatherMapClient::getDt(int index) {
  return weathers[index].dt;
}

String OpenWeatherMapClient::getCity(int index) {
  return weathers[index].city;
}

String OpenWeatherMapClient::getCountry(int index) {
  return weathers[index].country;
}

String OpenWeatherMapClient::getTemp(int index) {
  return weathers[index].temp;
}

String OpenWeatherMapClient::getTempRounded(int index) {
  return roundValue(getTemp(index));
}

String OpenWeatherMapClient::getHumidity(int index) {
  return weathers[index].humidity;
}

String OpenWeatherMapClient::getHumidityRounded(int index) {
  return roundValue(getHumidity(index));
}

String OpenWeatherMapClient::getCondition(int index) {
  return weathers[index].condition;
}

String OpenWeatherMapClient::getWind(int index) {
  return weathers[index].wind;
}

String OpenWeatherMapClient::getWindRounded(int index) {
  return roundValue(getWind(index));
}

String OpenWeatherMapClient::getWeatherId(int index) {
  return weathers[index].weatherId;
}

String OpenWeatherMapClient::getDescription(int index) {
  return weathers[index].description;
}

String OpenWeatherMapClient::getIcon(int index) {
  return weathers[index].icon;
}

boolean OpenWeatherMapClient::getCached() {
  return weathers[0].cached;
}

String OpenWeatherMapClient::getError() {
  return weathers[0].error;
}

// Map WMO weather codes to Meteocons font characters
String OpenWeatherMapClient::getWeatherIcon(int index) {
  int code = getWeatherId(index).toInt();
  String W = ")"; // default: N/A

  if (code == 0) W = "B";             // Clear sky -> Sun
  else if (code == 1) W = "B";        // Mainly clear -> Sun
  else if (code == 2) W = "H";        // Partly cloudy -> Cloudy
  else if (code == 3) W = "Y";        // Overcast -> Mostly cloudy
  else if (code == 45 || code == 48) W = "M"; // Fog
  else if (code >= 51 && code <= 57) W = "R"; // Drizzle -> Rain
  else if (code >= 61 && code <= 67) W = "R"; // Rain
  else if (code >= 71 && code <= 77) W = "W"; // Snow
  else if (code >= 80 && code <= 82) W = "R"; // Rain showers
  else if (code >= 85 && code <= 86) W = "W"; // Snow showers
  else if (code >= 95 && code <= 99) W = "0"; // Thunderstorm

  return W;
}
