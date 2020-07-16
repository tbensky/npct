### tl;dr

* This presents construction of an inexpensive (`~$20 USD`) hardware contact tracer.
* It does not use a mobile phone or the Internet at all.
* It uses an ESP32 and Bluetooth Low Energy to 1) broadcast your health information and 2) log the same from those around you.
* Contact tracing[(1)](https://www.cdc.gov/coronavirus/2019-ncov/php/open-america/contact-tracing/index.html)[(2)](https://www.who.int/publications/i/item/contact-tracing-in-the-context-of-covid-19) can help to get us out of the Covid-pandemic. 
* Build this, configure it and carry it around!

# The *No Phone* Contact Tracer (NPCT) Project

The goal of this work is to develop an open-source, hardware-based, anonymous contract tracing system that *doesn't use a mobile phone at all.*  Say hello to the "NPCT" (**n**o **p**hone **c**ontact **t**racer) Project:

<center><img src=https://github.com/tbensky/npct/blob/master/pics/3dcase.jpg></center>

## Why this project?

I developed this project starting in late June of 2020, because of the U.S. (red) line in this graph (source: [Johns Hopkins](https://coronavirus.jhu.edu/data/new-cases)):

<center><img src=https://github.com/tbensky/npct/blob/master/pics/new_cases01.png></center>

I wanted to focus some quarantine stress/energy and do something (anything) to help. I know a lot about microcontrollers, am good with the C language, and always seem to be able to figure out APIs. So I developed this open-source, hardware contract tracer token project. (I know...I know...Likely this project won't help at all, but here it is anyway.)

## Contact Tracing

Contact tracing is about you going out and about, and doing two things: 

1. Letting people you come in close proximity to know about your health condition and,

1. likewise you knowing the health condition of those same people. 

If you are feeling sick, others your encounter might benefit from knowing this, so they can act accordingly if they get sick a few days later.  Likewise, if you are around people who are sick, you might take appropriate actions if you start to feel sick.

So how do we do (1) and (2)?  Do we walk into a grocery store and yell out "I'm not feeling well...if you get sick in a couple of days, it might be because of me!" No, but likely using some technology to quietly inform those around us about our condition would work.

### Mobile Phones for Contact Tracing?

Mobile phones might be ideal for this. "Everyone already has one," and they can broadcast short Bluetooth messages about your health to those (phones) nearby for easy and automated tracking by all later on. 

 However, by Summer 2020, phone-based contact tracers were getting kind of [becoming a mess](https://www.forbes.com/sites/zakdoffman/2020/06/19/how-apple-and-google-created-this-contact-tracing-disaster/#3ce3f7797ca2). Many  of contact-tracing Apps were appearing, some by companies and individuals; others developed by governmental agencies for countrywide depolyment. Whose App do I use? Am I really doing anything? How do I register? Are they all compatible with each other? And most importantly: Who will have access to my (health) information? 

The (open-source) Apps revealed all kinds of tricks to keep the Apps running and tracing [in the background on a phone](https://github.com/NHSX). Google and Apple also [stepped in](https://www.apple.com/newsroom/2020/04/apple-and-google-partner-on-covid-19-contact-tracing-technology/) to unify the development process.  The Apps decreased battery life of the phones, but more importantly, trust in Apps and what companies do with your data was becoming a larger and larger question.  (The latest round is "clipboard spying" in stories such as [this one](https://www.computing.co.uk/news/4017082/tiktok-spying-clipboard-researchers-warn-iphone-users) and [this one](https://www.forbes.com/sites/daveywinder/2020/07/04/apple-ios-14-catches-microsofts-linkedin-spying-on-clipboard-tiktok-apps-privacy-iphone-ipad-macbook/#ecac5085896e) certainly didn't help.) It's also not clear you need an instantaenous notification if you encounter someone who is sick.

Phones and Apps are for fun and communication, but we will likely not trust them with health information. (If you want full privacy, you'd have to leave your phone off and at home.) 

### If not a phone, then what?

The maker movement continues to impress. They got a quick start in early 2020 backfilling PPE shortages with a mask and face shield producing efforts, like these: [masks](https://www.makethemasks.com) and [face shields](https://www.prusaprinters.org/prints/25857-prusa-face-shield). **Is a 'maker' themed contract tracer possible?**

Bluetooth (BT) does seems to be an ideal technology for contact tracing. It's a short-range protocol that can broadcast messages to receiving devices nearby. But I didn't want to to use a phone for this, so what then?  I could not help but wondering about the Arduino world and all of the small and low-priced boards out there. Even the good-old Arduino Uno is relatively portable and will run off of a 9V battery for half a day or so.  Plus, there are plenty of Arduino+BT options.  


One board that came to mind in particular was the [ESP32](https://esp32.com).  It costs $8....$8! This is cheaper than any Arduino, with or without BT, and it's a fully BT-enabled board about 2"x1" in size. Makers use it for all kinds of things, and $8 is pretty "no-risk."  (It is about what materials cost for masks people are making.)  There had to be some way of using an $8 device for a contact tracer. So I bought 3 on ebay and got to work. Here was my plan:

* Come up with some encoding for a unique ID for yourself + your health information (Easy: an MD5 hash + some additional codes to describe your health).

* Come up with a way of broadcasting such information to those who come near you.  (Easy again: put this encoding into the *BT name* of the ESP32, and tell it to constantly advertise itself.)  By putting it into the BT name, other devices can just "discover" each other and grab health info, so devices don't have to pair to exchange info.

* Come up with a way of monitoring the health of those nearby.  (Easy: As mentioned, tell the ESP32 to constantly discover advertising devices, and log ones that have a BT name that also encodes one's health info.) 

* Make it easy for one to set/change their health status, and to retrieve a log of other's health info the device found.

* Power it with a $10 portable "extra phone charge" battery.

* And, most importantly: No server, no storage, no App, no account to make, no Internet, no GPS.

That's it. It works, and here's how. Want to build one and try it out?

### So participants will need an ESP32 (and to carry one around)?

Yes, and this is likely a show stopper.  But an ESP32 is only $8 and people are making all kinds of things to adapt and get through the pandemic. Maybe it'll be fun to make and operate your own contact tracer? Also, there is some precedence for a (non-phone) [hardware device for contact tracing](https://simmel.betrusted.io). It's called a "contact tracing hardware token."

# How to build an ESP32 contact tracer

## Items needed

To build the contact tracer, you'll need three items: an [ESP32](https://www.ebay.com/itm/ESP32-ESP-32S-NodeMCU-Development-Board-2-4GHz-WiFi-Bluetooth-Dual-Mode-CP2102/382601606021?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649), a [micro-USB cable](https://www.amazon.com/gp/product/B07PFZDQP6/ref=ppx_yo_dt_b_asin_image_o00_s00?ie=UTF8&psc=1) (most of the time these come with the battery), and a [battery power pack](https://www.amazon.com/gp/product/B00MWU1GGI/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1). (The ones used here are pointed to by the links.)

<img src=https://github.com/tbensky/npct/blob/master/pics/all3.jpg>

## Assembly

Assembling it is just a matter of connecting the ESP32 to the battery as shown. There is no soldering or wiring required at all.

<img src=https://github.com/tbensky/npct/blob/master/pics/unit.JPG>

The ESP32 should be protected. It can be placed or wrapped in anything non-metallic. There are 3D printable cases for it, like [this one](https://www.thingiverse.com/thing:3195951). Do not put it in an Altoids box (or *anything* metallic) for two reasons: 1) It could short out a pin or two and 2) the Bluetooth connectivity would be severely degraded (the "bag trick" [here](https://youtu.be/s3poKUuvtyM?t=55) would actually work). 

Here's the assembly in a 3D printed:

<img src=https://github.com/tbensky/npct/blob/master/pics/3dcase.jpg>

All told, wow! Throwing these parts together is very portable, and a kind of viable "product."  This form could easily be tossed into a bag, backpack, or even pocket and be used as a portable contract tracer.  No soldering or wiring is required at all.


## Software

Software needed has two aspects. The first is software that goes on the ESP32 to run the contact tracing. The second is local software to allow a user to configure the ESP32 and retrieve contact tracing information.


### ESP32 Software

The software for the ESP works to implement "the plan" bullet points above. It accomplishes two primary functions:

* Starts a Bluetooth low-energy (BLE) server. This allows for outside connection to the ESP32 device by the user for configuration and reading of the device. "Configuration" here means to be able to set the BLE name of the ESP32 with a unique ID and health-status code (of the user). This BLE name (hence health information) is continually advertised by the ESP32. "Reading" here means to retrieve health related contacts that may have been received. 

* Starts a BLE discovery scan, to continually scan for the names of other nearby BLE devices. All contact tracing names as used by this system, start with the hash-tag "#C19:".  Any names not starting with this are ignored. Those starting with it are logged into the ESP32's internal RAM. About 5,000 such names can be logged.

The software is located in the [npct folder](https://github.com/tbensky/npct/tree/master/npct) and is a full blown ESP32 project developed using the Espressif IoT Development Framework (esp-idf).

### Local Software

Conveniently connecting with a device over BLE sort of requires a custom application to be developed (which I didn't want to do). I also wanted to leave the phone out of this.  Luckily, 
Google Chrome has [Web Bluetooth](https://developers.google.com/web/updates/2015/07/interact-with-ble-devices-on-the-web) built into it. It allows a browser (via Javascript) to communicate with BLE devices.

So an HTML/Javascript configuration tool for this contact tracer was developed and is found [here, called config.html](https://github.com/tbensky/npct/tree/master/configapp). It looks like this:

<img src=https://github.com/tbensky/npct/blob/master/pics/chrome.png>


# Using the contact tracer

## Flashing the ESP32 Device

The contact tracing software needs to be flashed onto the ESP32 device. This only has to be once. You can certainly download and install the Espressif IoT Development Framework (esp-idf), then compile, and flash the code to the board yourself. Or, you can just [grab the binary](https://github.com/tbensky/npct/blob/master/npct/build/npct.bin) from this repository and flash it using any number of
"flash utilities" out there. To do this:

1. Download either the Windows or macOS flashing utility [here](https://github.com/doayee/esptool-esp32-gui/releases).  DoayeeESP32DFU.app.zip is for macOS and DoayeeESP32DFU.exe is for Windows. 

1. Connect the ESP32 to your computer using a USB-A to micro-USB cable.

1.  When this flashing utility is run, you'll see <img src=https://github.com/tbensky/npct/blob/master/pics/flash.png>

1. Select the serial port your ESP32 is attached to. This varies from computer to computer.

1. Click the "Browse..." button next to the "Flash App 0x10000" button (red arrow), and select the file called [npct.bin](https://github.com/tbensky/npct/blob/master/npct/build/npct.bin) file in the ncpt repository.  

1.  Now click the "Flash" button.  

1. When the "Connecting........_____....." message starts up, press and hold the "Boot" button on the ESP32 until you see "Writing .... (10%)..." such messages.  (Boot button: hold the ESP32 so both buttons and the USB connector point "up," and the face of the board points toward you. The boot button is to the left of the USB connector.)

That's it. The ESP32 is ready to use as a contact tracer, and these steps do not need to be done again.

## Configuring the ESP32

Now configure the contact tracer itself. (This has only been tested on macOS Catalina and Windows 10.)  

1. Get the latest version of Google Chrome (83+). 

1. Keeping all files in the [configapp](https://github.com/tbensky/npct/tree/master/configapp) folder together, in Chrome do a "File->Open..." on `config.html`.

1. The first time you run it, click the green "Generate ID" button to get your unique contact tracing ID.

1. Tick on any relevant health information.

1. Click on "Update hardware." This should allow you to connect to the ESP32 via BLE and it will write your configuration information to it.

1. Sometimes you might have trouble getting Chrome to "see" ESP32. If so, type `chrome://bluetooth-internals/#devices` into the URL location field, and try to connect to the ESP32 manually. When you do so, go back to `config.html` and it should work. 


After you configure the ESP32 for the first time, its BT name will change to reflect your unique ID and health info for other contact tracers to pick up on. It's name will be something like `#C19:abcdefghijklmnop17`.

Come back to `config.html` as needed to update your health information or query the device for contacts that may have occurred while you were out.

# Participate in contact tracing

You are done. Power the ESP32 using a battery and take it with you when you go out! The battery should power it for 40+ hours. Be sure not to disconnect it while you're out and about, as any 
logged contact information will be lost.  The blue LED will come on if any health-related contacts are logged, so no need to check it if you get home and the blue LED is off (the red LED is always on).

Be sure to re-visit `config.html` as your health situation changes. Also you can use `config.html` to read any contact information that came in while you were out.



# Technical notes

* The ESP32 in full BLE mode seems to draw 112 mA. I measured it using this rock'in USB cable I cut into and modified so current can be routed into a meter. With a 5000mA battery, the ESP32 should run for 44 hours (a couple of days) on a single charge.

<center><img src=https://github.com/tbensky/npct/blob/master/pics/current.jpg></center>

(I would have used my Fluke to measure the current, but the fuse ($$$) in it is blown, so I used this free meter from HFT.)

* The ESP32 code, [npct.c](https://github.com/tbensky/npct/blob/master/npct/main/npct.c) is a mashup of two example files in the ESP32 development package: [gatts_demo.c](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/gatt_server/main/gatts_demo.c) and [gattc_demo.c](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/gatt_client/main/gattc_demo.c). The server demo (gatt**s**) handles incoming BLE requsts on a couple of pseudo BLE services.  The client demo, gatt**c** scans for the server's services in BLE space and connects to it. So I pared down the server to just one service and characteristic, and pulled the BLE name scanning out of the client, then combine it all into one. The code needs more refactoring and pruning of code that isn't needed. Kudos to the ESP32 team for providing such a useful set of complete, and comprehensive examples (that actually work). Thanks also to two people I never met or communcationed with for various BLE postings around Github and the web: [nkolban](https://github.com/nkolban) and [chegewara](https://github.com/chegewara).

* The ID system used here works as follow.

	1. Everyone starts by running `config.html` to configure the ESP32. The first time though, An md5 is computed based on as many random things as I could think of in `config.html`. This initial md5 is their `private code.`  Another md5 is then calculated from `some salt` + `private code.`  The 32 character length of this 2nd md5 is their `public code`, but it is a tad long for a BLE name, so it is cut in half to 16 (I read somwhere that entropy is still pretty uniform in such a thing). 

	1. The ESP32's BLE name will be the tag `#C19:` + `public code` + a 2-digit hex code about their health. So something like `#C19:abcdefghijklmnop06` for someone's who public md5 is `abcdefghijklmnop` and has a sore throat and a cough (see `config.html` for "important" health codes I pulled from the CDC).

	1. The `private code` is used to verify the person tossing around the public md5 is also the person who ran `config.html` and got all of this going in the first place. That is (in particular for sharing their log data online) they might be asked for both md5s, and only if `substr(md5(salt+private),16) == public` do we believe them. I am guessing that knowing the salt (it's in the source code) and the public code will not allow anyone to "compute" the private md5. (But who cares....this isn't a bank--the whole system relies on everyone "playing nice" to help us get out of this damn pandemic.) 

	1. Users are asked to store both codes on their own computer and are also stored in "local storage" in Chrome for convenience.

* Testing the ESP32 logging.

	* In function `gap_event_handler`, the case `ESP_GAP_BLE_SCAN_RESULT_EVT` means the ESP32 found a BLE name and it should be added to a dynamic memory location called `Encounters` for later retrieval.


