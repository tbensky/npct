# The *No Phone* Contact Tracer (NPCT) Project

The goal of this work is to develop an open-source, hardware-based, anonymous contract tracing system that *doesn't use a mobile phone at all.*  Say hello to the "NPCT" (**n**o **p**hone **c**ontact **t**racer) Project.

## Contact Tracing

Contact tracing is about you going out and about, and doing two things: 

1. Letting people you come in close proximity to know about your health condition and,
2. likewise you knowing the health condition of those same people. 

If you are feeling sick, others would benefit from knowing and perhaps acting accordingly if they get sick a few days later.  Likewise, if you are around people who are sick, you might take appropriate actions if you start to feel sick.

So how do we do (1) and (2)?  Do we walk into a grocery store and yell out "I'm not feeling well...if you get sick in a couple of days, it might be because of me!" No, but likely using some technology to quietly inform those around us about our condition would work.

### Mobile Phones for Contact Tracing?

Mobile phones could be ideal for this. "Everyone already has one," and they can broadcast short Bluetooth messages about your health to those (phones) nearby for easy and automated tracking by all later on. 

By Summer 2020, phone-based contact tracers were getting kind of stuck. Many  of contact-tracing Apps were appearing, some by companies and individuals; others by countries themselves. Whose App do I use? Am I really doing anything? How do I register? Are they all compatible with each other? And most importantly: Who will have access to my (health) information? 

Open-source Apps revealed all kinds of tricks to keep the Apps running and tracing [in the background on a phone](https://github.com/NHSX).  Sounds good, but battery life was decreasing and trust in Apps and what companies do with your data is always in question.  The latest round is "clipboard spying" in stories such as [this one](https://www.computing.co.uk/news/4017082/tiktok-spying-clipboard-researchers-warn-iphone-users) and [this one](https://www.forbes.com/sites/daveywinder/2020/07/04/apple-ios-14-catches-microsofts-linkedin-spying-on-clipboard-tiktok-apps-privacy-iphone-ipad-macbook/#ecac5085896e). 

Phones and Apps are for fun and communication, but we will likely not trust them with health information. (If you want full privacy, you'd have to leave your phone off and at home.) 


### Bluetooth

Bluetooth (BT) seems to be an ideal technology for contact tracing. It's a short-range protocol that can broadcast messages to receiving devices nearby. But I didn't want to to use a phone for this, so what's the plan?

The maker movement continues to impress. They got a quick start in backfilling PPE shortages with a mask-making effort, like this one [masks](https://www.makethemasks.com) and [face shields](https://www.prusaprinters.org/prints/25857-prusa-face-shield). So maybe some 'maker' themed contract tracer would work?  

$8....$8. A fully BT-enabled [ESP32](https://esp32.com) is $8 on eBay. Makers use these for all kinds of things, and $8 is about what materials cost for masks people are making.  There had to be some way of using an $8 device for a contact tracer.  So here's my plan:

* Come up with some encoding for a unique ID for yourself + your health information (Easy: an MD5 + some additional hex codes to describe your health).

* Come up with a way of broadcasting your information to those who come near you.  (Easy again: put this encoding into the *BT name* of the ESP32, and tell it to constantly advertise itself.)  By putting it into the BT name, devices don't have to pair to exchange info.

* Come up with a way of monitoring the health of those nearby.  (Easy: Tell the ESP32 to constantly discover advertising devices, and log ones that have a BT name that also encodes one's health info.) 

* Make it easy for one to set/change their health status, and to retrieve a log of other's health info the device found.

* Power it with a $10 portable "extra phone charge" battery.

* No server, no storage, no App, no account to make, no Internet required.

That's it. It works, and here's how. Want to build one and try it out?

### So participants will need an ESP32 (and to carry one around)?

Yes, and this is likely a show stopper.  But an ESP32 is only $8 and people are making all kinds of things to adapt and get through the pandemic. Maybe it'll be fun to make and operate your own contact tracer? Also, there is some precedence for a (non-phone) [hardware device for contact tracing](https://simmel.betrusted.io). It's called a "contact tracing hardware token."

# How to build an ESP32 contact tracer

## Items needed

To build the contact tracer, you'll need three items: an [ESP32](https://www.ebay.com/itm/ESP32-ESP-32S-NodeMCU-Development-Board-2-4GHz-WiFi-Bluetooth-Dual-Mode-CP2102/382601606021?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649), a [micro-USB cable](https://www.amazon.com/gp/product/B07PFZDQP6/ref=ppx_yo_dt_b_asin_image_o00_s00?ie=UTF8&psc=1) (most of the time these come with the battery), and a [battery power pack](https://www.amazon.com/gp/product/B00MWU1GGI/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1). (The ones used here are pointed to by the links.)

<img src=https://github.com/tbensky/npct/blob/master/pics/all3.jpg>

## Assembly

Assembling it is just a matter of connecting the ESP32 to the battery like this

<img src=https://github.com/tbensky/npct/blob/master/pics/unit.JPG>

The ESP32 should be protected some. It can be wrapped in anything non-metallic, or try to 3D print a case for it, like [this one](https://www.thingiverse.com/thing:3195951). Do not put in an Altoids box (or *anything* metallic) for two reasons: 1) It could short out a pin or two and 2) kill the Bluetooth connectivity (the "bag trick" [here](https://youtu.be/s3poKUuvtyM?t=55) would actually work).


## Software

Software needed has two aspects. The first is software that goes on the ESP32 to run the contact tracing. The second is local software to allow a user to configure the ESP32 and retrieve contact tracing information.


### ESP32 Software

The software for the ESP works to implement "the plan" bullet points above. It accomplishes two primary functions:

* Starts a Bluetooth low-energy (BLE) server. This allows for outside connection to the ESP32 device by the user for configuration and reading of the device. "Configuration" here means to be able to set the BLE name of the ESP32 with a unique ID and health-status code (of the user). This BLE name (hence health information) is continually advertised by the ESP32. "Reading" here means to retrieve health related contacts that may have been received. 

* Starts a BLE discovery scan, to continually scan for the names of other nearby BLE devices. All contact tracing names as used by this system, start with the hash-tag "#C19:".  Any names not starting with this are ignored. Those starting with it are logged into the ESP32's internal RAM. About 5,000 such names can be logged.

The software is located in the [npct folder](https://github.com/tbensky/npct/tree/master/npct) and is a full blown ESP32 project developed using the Espressif IoT Development Framework (esp-idf).

### Local Software

Conveniently connecting with a device over BLE sort of requires a custom application to be developed (which we didn't want to do). We also wanted to leave the phone out of this.  Luckily, 
Google Chrome has [Web Bluetooth](https://developers.google.com/web/updates/2015/07/interact-with-ble-devices-on-the-web) built into it. It allows a browser (via Javascript) to communicate with BLE devices.

So an HTML/Javascript configuration tool for this contact tracer was developed and is found [here, called config.html](https://github.com/tbensky/npct/tree/master/configapp). It looks like this:

<img src=https://github.com/tbensky/npct/blob/master/pics/chrome.png>


# Using the contact tracer

## Flashing the ESP32 Device

The contact tracing software needs to be flashed onto the ESP32 device. This only has to be once.  To do this:

1. Download either the Windows or macOS flashing utility [here](https://github.com/doayee/esptool-esp32-gui/releases).  DoayeeESP32DFU.app.zip is for macOS and DoayeeESP32DFU.exe is for Windows. 

1. Connect the ESP32 to your computer using a USB-A to micro-USB cable.

1.  When run, you'll see <img src=https://github.com/tbensky/npct/blob/master/pics/flash.png>

1. Select the serial port your ESP32 is attached to. This varies from computer to computer.

1. Click the "Browse..." button next to the "Flash App 0x10000" button (red arrow), and select the file called [npct.bin](https://github.com/tbensky/npct/blob/master/npct/build/npct.bin) file in the ncpt repository.  

1.  Now click the "Flash" button.  

1. When the "Connecting...." message starts up, press and hold the "Boot" button on the ESP32 until you see "Writing .... (10%)..." such messages.  

That's it. The ESP32 is ready to use as a contact tracer, and this step does not need to be done again.

## Configuring the ESP 



