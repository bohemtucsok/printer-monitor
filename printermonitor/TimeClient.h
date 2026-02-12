/**The MIT License (MIT)

Copyright (c) 2015 by Daniel Eichhorn

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

See more at http://blog.squix.ch
*/

/*
Modified by David Payne for use in the Scrolling Marquee
Modified to use NTP with POSIX timezone for automatic DST handling
*/

#pragma once

#include <ESP8266WiFi.h>
#include <time.h>

class TimeClient {

  private:
    String myTimeZone;
    boolean timeSynced = false;

  public:
    TimeClient(String timeZone);
    void updateTime();

    void setTimezone(String timeZone);
    String getHours();
    String getAmPmHours();
    String getAmPm();
    String getMinutes();
    String getSeconds();
    String getFormattedTime();
    String getAmPmFormattedTime();
    long getCurrentEpoch();
    long getCurrentEpochWithUtcOffset();

};
