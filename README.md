### tl;dr

* Participating in contact tracing[(1)](https://www.cdc.gov/coronavirus/2019-ncov/php/open-america/contact-tracing/index.html)[(2)](https://www.who.int/publications/i/item/contact-tracing-in-the-context-of-covid-19) is something each of us can do to help slow the spread the Covid-pandemic. 
* This work shows an inexpensive (`~$20 USD`) hardware-based contact tracer.
* This contact tracer *does not use a mobile phone at all*.
* If you build this and carry this around, you can participate in *self-driven* contact tracing.
* It uses an ESP32 and Bluetooth Low Energy to 1) broadcast your health information and 2) log the same from those around you.

# The *No Phone* Contact Tracer (NPCT) Project

## <a href="https://github.com/tbensky/npct/blob/master/AssemblyInstructions/instructions.pdf">Short assembly instructions</a>

## Project Goals
The goal of this work is to develop an open-source, hardware-based, anonymous and "self-driven" contract tracing system that *doesn't use a mobile phone at all.*  Say hello to the "NPCT" (**n**o **p**hone **c**ontact **t**racer) Project:

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/3dcase.jpg></p>

## Why this project?

I developed this project starting in late June of 2020, because of the red line in this graph (source: [Johns Hopkins](https://coronavirus.jhu.edu/data/new-cases)):

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/new_cases01.png width=500></p>

I wanted to focus some quarantine stress/energy and do something (anything) to help. I took [this course](https://www.coursera.org/learn/covid-19-contact-tracing?edocomorp=covid-19-contact-tracing), and mixed it all with my knowledge of microcontrollers, the C language, and being be able to figure out APIs. So I developed this open-source, hardware contract tracer token project. (I know...I know...likely this project won't help at all, but here it is anyway. Contact me (see email [here](https://github.com/tbensky)) if you'd like to talk about it.)

## Contact Tracing

Contact tracing is a mechanism for slowing the spread of an infectious disease.  It works like this.

Suppose someone gets infected with the disease. They're called "the case." The case should let everyone they've been in recent contact with (the contacts) know that they're sick.  The case should isolate and the contacts should quarantine. This can help stop the spread of an infectious disease.

But this requires a lot of legwork. In the contact tracing protocol, typically carried out by public health, those who test positive (each now a "case") will be contacted (by public health) to tell them of/confirm their test result. They will be asked to isolate. Additionally, public health will inquire about the case's recent contacts. Public health will then contact the contacts, discuss with them about going into quarantine, etc. and on it goes.  (Then there's the hardships of isolating and quarantining.)

Typically contact tracing is all done by phone. It is slow and arduous work, but time is of the essence, as a contact (if they then become sick) will likely themselves become infectious within days, spreading the disease to those *they* contact. Numbers for COVID-19: People are infectious 2-days before showing symptonms, 50% show symptoms within 5 days, infected people should isolate for 10-days, contacts should quarantine for 2-weeks.

### Can technology help? Can we do self-driven contact tracing? 

What if you go out and about, and do two things: 

1. Let people you come in close proximity with about your health condition and,

1. likewise yourself learn about the health condition of those same people. 

If you are feeling sick, others your encounter might benefit from knowing this, so they can act accordingly if they get sick a few days later.  Likewise, if you are around people who are sick, you might take appropriate actions if you start to feel sick.

Would this be a way for us all to participate in self-driven contact tracing?

To start, how do we do (1) and (2)?  Do we walk into a grocery store and yell out "I'm not feeling well...if you get sick in a couple of days, it might be because of me!" No. And [this](https://youtu.be/kSSibWkQcTA?t=130) won't do it either. 

Using some technology to quietly inform those around us about our condition *might* work.

### Mobile Phones for Contact Tracing?

Mobile phones might be ideal for this. "Everyone already has one," and they can broadcast short Bluetooth messages about your health to those (phones) nearby for easy and automated tracking by all later on. 

However, by Summer 2020, phone-based contact tracers were getting kind of becoming a mess: [1](https://www.forbes.com/sites/zakdoffman/2020/06/19/how-apple-and-google-created-this-contact-tracing-disaster/#3ce3f7797ca2), [2](https://www.nytimes.com/2020/07/21/technology/korea-coronavirus-app-security.html), [3](https://www.nytimes.com/2020/07/08/technology/virus-tracing-apps-privacy.html). Many contact-tracing Apps were appearing, some by companies and individuals; others developed by governmental agencies for countrywide deployment. Whose App do I use? Am I really doing anything? How do I register? Are they all compatible with each other? And most importantly: Who will have access to my (health) information? 

The (open-source) Apps revealed all kinds of tricks to keep the Apps running and tracing [in the background on a phone](https://github.com/NHSX). Google and Apple also [stepped in](https://www.apple.com/newsroom/2020/04/apple-and-google-partner-on-covid-19-contact-tracing-technology/) to unify the development process, but it's not clear their efforts helped.  The Apps decreased battery life of the phones, but more importantly, trust in Apps and what companies do with your data was becoming a larger and larger question.  (The latest round is "clipboard spying" in stories such as [this one](https://www.computing.co.uk/news/4017082/tiktok-spying-clipboard-researchers-warn-iphone-users) and [this one](https://www.forbes.com/sites/daveywinder/2020/07/04/apple-ios-14-catches-microsofts-linkedin-spying-on-clipboard-tiktok-apps-privacy-iphone-ipad-macbook/#ecac5085896e) certainly didn't help.) It's also not clear you need an *instantaenous notification* if you encounter someone who is sick.

Phones and Apps are for fun and communication, but we will likely not trust them with health information. (If you want full privacy, you'd have to leave your phone off and at home.[[ref.]](https://www.bunniestudios.com)) 

### If not a phone, then what?

The maker movement continues to impress. They got a quick start in early 2020 backfilling PPE shortages with a mask and face shield producing efforts, like these: [masks](https://www.makethemasks.com) and [face shields](https://www.prusaprinters.org/prints/25857-prusa-face-shield). **Is a 'maker' themed contract tracer possible?**

Bluetooth (BT) does seem to be an ideal technology for contact tracing. In particular, Bluetooth Low Energy (BLE). Both are a short-range protocols that can broadcast messages to receiving devices nearby. BLE is meant for *short* messages, like "I'm sick."  But I didn't want to to use a phone for this, so what then?  I could not help but wonder about the Arduino world and all of the small and low-priced boards out there. Even the good-old Arduino Uno is relatively portable and will run off of a 9V battery for half a day or so.  Plus, there are plenty of Arduino+BT options.  

One board that came to mind in particular was the [ESP32](https://www.espressif.com).  It costs $8....$8! This is cheaper than any Arduino, with or without BT, and it's a fully BT-enabled board about 2"x1" in size. Makers use it for all kinds of things, and $8 is pretty "no-risk."  (It is about what materials cost for masks people are making.)  There had to be some way of using an $8 device for a contact tracer. So I bought 3 on ebay and got to work. 

Here was my plan that I would implement using BLE and the ESP32:

* Come up with a way of coding an anonymous ID and health information for each participant.

* Come up with a way of broadcasting such information to those who come near you.  

* Come up with a way of recieving health information of those nearby.  

* Make it easy for one to set/change their health status, and to retrieve a log of the health information of those you encounter. 

* Power it with a $10 portable "extra phone charge" battery.

* And, most importantly: No server, no storage, no App, no account to make, no Internet, no GPS.

That's it. I put all of this together using an ESP32 and Web-Bluetooth in the Chrome browser.  It all works, and here's how. Want to build one and try it out?

### So participants will need an ESP32 (and to carry one around)?

Yes, and this is likely a show stopper.  But an ESP32 is only $8 and people are making all kinds of things to adapt and get through the pandemic. Maybe it'll be fun to make and operate your own contact tracer? Also, there is some precedence for a (non-phone) [hardware device for contact tracing](https://simmel.betrusted.io). It's called a "contact tracing hardware token." Singapore apparently [developed one](https://www.bbc.com/news/technology-53146360) too.

# How to build an ESP32 self-driven contact tracer

## Items needed

To build this contact tracer, you'll need three items: an [ESP32](https://www.ebay.com/itm/ESP32-ESP-32S-NodeMCU-Development-Board-2-4GHz-WiFi-Bluetooth-Dual-Mode-CP2102/382601606021?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649), a [battery power pack](https://www.amazon.com/gp/product/B00MWU1GGI/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1), and a [micro-USB cable](https://www.amazon.com/gp/product/B07PFZDQP6/ref=ppx_yo_dt_b_asin_image_o00_s00?ie=UTF8&psc=1) (most of the time these come with the battery). (The items I used are pointed to by the links.)

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/all3.jpg></p>

## Assembly

Assembling it is just a matter of connecting the ESP32 to the battery as shown. There is no soldering or wiring required at all.

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/unit.JPG></p>

The ESP32 should be protected. It can be placed or wrapped in anything non-metallic. There are 3D printable cases for it, like [this one](https://www.thingiverse.com/thing:3195951). Do not put it in an Altoids box (or *anything* metallic) for two reasons: 1) It could short out a pin or two and 2) the Bluetooth connectivity would be severely degraded (the "bag trick" [here](https://youtu.be/s3poKUuvtyM?t=55) would actually work). 

Here's the assembly in a 3D printed:

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/3dcase.jpg></p>

All told, wow! Throwing these parts together is very portable, and a kind of viable "product."  This form could easily be tossed into a bag, backpack, or even pocket and be used as a portable contract tracer.  No soldering or wiring is required at all.

No on/off button or mass storage is needed. The battery will run it for about two days, and it can store up to 5,000 unique contacts.


## Software

Software needed has two aspects. The first is software that goes on the ESP32 to run the contact tracing. The second is local software to allow a user to configure the ESP32 and retrieve contact tracing information.


### ESP32 Software

The software for the ESP works to implement "the plan" bullet points above. It accomplishes two primary functions:

* Starts a Bluetooth low-energy (BLE) server. This allows for outside connection to the ESP32 device by the user for configuration and reading of the device. "Configuration" here means to be able to set the BLE name of the ESP32 with a unique ID and health-status code (of the user). This BLE name (hence health information) is continually advertised by the ESP32. "Reading" here means to retrieve health related contacts that may have been received. 

* Starts a BLE discovery scan, to continually scan for the names of other nearby BLE devices. All contact tracing names as used by this system, start with the hash-tag "#C19:".  Any names not starting with this are ignored. Those starting with it are logged into the ESP32's internal RAM. About 5,000 such names can be logged.

The software is located in the [npct folder](https://github.com/tbensky/npct/tree/master/npct) and is a full blown ESP32 project developed using the Espressif IoT Development Framework (esp-idf).

### Local Software

Conveniently connecting with a device over BLE sort of requires a custom application to be developed (which I didn't really want to do). I also wanted to leave the phone out of this.  Luckily, 
Google Chrome has [Web Bluetooth](https://developers.google.com/web/updates/2015/07/interact-with-ble-devices-on-the-web) built into it. It allows a browser (via Javascript) to communicate with BLE devices.

So an HTML/Javascript configuration tool for this contact tracer was developed and is found [here, called config.html](https://github.com/tbensky/npct/tree/master/configapp). It looks like this:

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/chrome.png></p>


# Using the contact tracer

## Flashing the ESP32 Device

The contact tracing software needs to be flashed onto the ESP32 device. This only has to be once. You can certainly download and install the Espressif IoT Development Framework (esp-idf), then compile, and flash the code to the board yourself. Or, you can just [grab the binary](https://github.com/tbensky/npct/blob/master/npct/build/npct.bin) in this repository and flash it using any number of
"flash utilities" out there. To do this:

1. [Here's](https://github.com/doayee/esptool-esp32-gui/releases) a very nice flashing utility.  Download for Windows or macOS:  DoayeeESP32DFU.app.zip is for macOS and DoayeeESP32DFU.exe is for Windows. 

1. Connect the ESP32 to your computer using a USB-A to micro-USB cable.

1.  When this flashing utility is run, you'll see 

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/flash.png></p>

1. Select the serial port your ESP32 is attached to. This varies from computer to computer.

1. Click the "Browse..." button next to the "Flash App 0x10000" button (red arrow), and select the file called [npct.bin](https://github.com/tbensky/npct/blob/master/npct/build/npct.bin).  

1.  Now click the "Flash" button.  

1. When the "Connecting........_____....." message starts up, press and hold the "Boot" button on the ESP32 until you see "Writing .... (10%)..." such messages.  The "boot button" is this one:

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/esp32_buttons.jpg width="320"></p>



That's it. The ESP32 is ready to use as a contact tracer, and these steps do not need to be done again.

## Configuring the ESP32

Now configure the contact tracer itself. (This has only been tested on macOS Catalina and Windows 10.)  

1. Get the latest version of Google Chrome (83+). 

1. Keeping all files in the [configapp](https://github.com/tbensky/npct/tree/master/configapp) folder together, in Chrome do a "File->Open..." on `config.html`.

1. The first time you run it, click the green "Generate ID" button to get your unique contact tracing ID.

1. Tick on any relevant health information.

1. Click on "Update hardware." This should allow you to connect to the ESP32 via BLE and it will write your configuration information to it.

1. Sometimes you might have trouble getting Chrome to "see" ESP32. Initally the BLE name of it will `ContactTracer`.  If you do have trouble, type `chrome://bluetooth-internals/#devices` into the URL location field, and try to scan for and "inspect" the ESP32 manually. When you do so, go back to `config.html` and it should work. 


After you configure the ESP32 for the first time, its BLE name will change to reflect your unique ID and health info for other contact tracers to pick up on. It's name will be something like `#C19:abcdefghijklmnop17`.

Come back to `config.html` as needed to update your health information or query the device for contacts that may have occurred while you were out.

# Participate in self-driven contact tracing

You are done. Now, here's what you do:

## Update your health information

Regularly update your health information on the contact tracer device.

## Take the contact tracer device with you

Power the ESP32 using a battery and take it with you when you go out! The battery should power it for 40+ hours. Be sure not to disconnect it while you're out and about, as any 
logged contact information will be lost.  You can disconnect it and/or charge the battery after you get home and download any contacts.

## Download any encounters

When you get home, check the for blue LED. It'll be on if any health-related contacts were logged. (The red LED is always on.) To read out your contact log, go back to the Chrome-App `(config.html)` to read any contact information that came in while you were out.  Click on the blue "read device log" button:

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/readlog.png width=400></p>

It'll pull and list all contacts you encountered. Nothing is saved or sent to any server. Be sure to cut and paste the enounters into a document on your computer (in Word perhaps?) if you want to save them.


## The tracer told me I was around someone with Covid-related symptoms.  Now what? 

Good question. 

Any answer in the context of contact tracing will be careful to avoid any *medical advice*. Contact tracers are not doctors and we're certainly not either.  (We're also not epidemiologists, but did complete [this course](https://www.coursera.org/learn/covid-19-contact-tracing?edocomorp=covid-19-contact-tracing)).  

Contact tracing is more about making an informed decision about what to do next, given you were near someone who is claiming to be sick and maybe contagious. You'll have to do some thinking now.

* You know what you were doing when the encounter came in. Were you shoulder-to-shoulder at a street party?  Or moving quickly through a large warehouse store shopping?

* At least for Covid-19, the danger is being less than 6 feet for more than 15 minutes. Could this have happened? Were you (and/or) any of your contacts not wearing a mask? 

* The log will tell you the number of times each encounters happened. One encounter is likely just a quick passer by.  But many? 10? 20? Maybe that is a 6/15 violation.

* The log gives you information about people you were around and one is claiming to have symptoms. *Take some steps* to stop the spread:

	* Change your plans and lie low for a couple of days. See if symptoms appear. Remember, we're in a pandemic. You don't need an excuse to hide out.

	* Gently quarantine yourself: wear a mask, even at home, and distance within your home (if possible).

	* Keep careful track of your contacts.

	* Have your doctor or testing station on speed-dial.  Make the call if any symptoms appear.

	* Resume your normal activities only when something definitive comes along clearing you to do so.

* See [here](https://www.cdc.gov/coronavirus/2019-ncov/php/principles-contact-tracing.html) under "Time is of the essence."

* What we know this: 

	* Around July 2020, the virus is spreading. If you were around (within 6 ft) of someone who is sick (for 15 min or more), it may have spreaded to you.

	* 50% of people show symptoms of Covid-19 within 5 days of being infected.

	* Infected people are contagious 2-days before they show symptoms.

* Do we trust this self-driven contact tracing system? We don't know. We hope someone isn't going through the trouble of configuring it and carrying it around, only to lie about their condition.

* More logic given your information:

	* Go to [rt.live](https://rt.live), and check the value of r0 (r-naught) for your state. Suppose it's 1.15. This means every infected person is infecting 1.15 other people.

	* You could be the 1 person someone else just infected, or you could be the 0.15 person to be infected (0.15=15% chance of becoming infected, or if the sick person gets around 6 more people (1/.15 is about 7...you + 6 others), odds are one of you will get infected.

 
* Be sure to re-use `config.html` to reconfigure your contact tracing device, as your health situation changes. 

## Next: Make another contact tracer for someone else

Next, make this contact tracer project for someone you know, and tell them to make one for someone they know. :smiley:




# Technical notes

* On the bullet point implementation plan above:

	* A unique ID and health information is a randomly generated MD5 hash with some additional digits tacked on to describe one's health. 

	* Broadcasting one's ID and health information to those nearby, is done by putting the ID+health codes into the *BLE name* of the ESP32, and then telling it to advertise itself.

	* Monitoring health of others nearby can be done by discovering BLE names of other advertising devices. There are a lot of BLE devices out there, but we reject those not in our ID+health code format.  If recognized, we simply grab and log the names (ID+health code) on the recieving device. (This way, devices don't have to pair to exchange info.)

	* A configuration and log retrieval App is a local Chrome (i.e. non-server based) `.html` page that uses Chrome's Web-Bluetooth implementation oto exchange BLE information with the ESP32.

* The ESP32 in full BLE mode seems to draw 112mA. I measured it using this rockin' USB cable I cut into and modified so current can be routed into a meter. With a 5000mA battery, the ESP32 should run for 44 hours (a couple of days) on a single charge.

<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/current.jpg></p>

(I would have used my Fluke to measure the current, but the fuse ($$$) in it is blown, so I used this free meter from HFT.)

* The ESP32 code, [npct.c](https://github.com/tbensky/npct/blob/master/npct/main/npct.c) is a mashup of two example files in the ESP32 development package: [gatts_demo.c](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/gatt_server/main/gatts_demo.c) and [gattc_demo.c](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/gatt_client/main/gattc_demo.c). The server demo (gatt**s**) handles incoming BLE requsts on a couple of pseudo BLE services.  The client demo, gatt**c** scans for the server's services in BLE space and connects to it. So I pared down the server to just one service and characteristic, and pulled the BLE name scanning out of the client, then combine them all into one. The code needs more refactoring and pruning of code that isn't needed. Kudos to the ESP32 team for providing such a useful set of complete, and comprehensive examples (that actually work). Thanks also to two people I never met or communcationed with for various BLE postings around Github and the web: [nkolban](https://github.com/nkolban) and [chegewara](https://github.com/chegewara).

* The ESP32 is pretty awesome. It works and has great documentation and sample programs, etc.

* The ID system used here works as follow.

	1. Everyone starts by running `config.html` to configure the ESP32. The first time though, An md5 is computed based on as many random things as I could think of in `config.html`. This initial md5 is their `private code.`  Another md5 is then calculated from `some salt` + `private code.`  The 32 character length of this 2nd md5 is their `public code`, but it is a tad long for a BLE name, so it is cut in half to 16 (I read somwhere that entropy is still pretty uniform in such a thing). 

	1. The ESP32's BLE name will be the tag `#C19:` + `public code` + a 4-digit hex code about their health (so up to 32 health conditions can be presented). So something like `#C19:abcdefghijklmnop0006` for someone whose public md5 is `abcdefghijklmnop` and has a sore throat and a cough (see `config.html` for relevant health codes pulled from the CDC).

	1. The `private code` is used to verify the person tossing around the public md5 is also the person who initially ran `config.html` and got all of this going in the first place. That is (in particular for sharing their log data online) they might be asked for both md5s, and only if `substr(md5(salt+private),16) == public` do we believe them. I am guessing that knowing the salt (it's in the source code) and the public code will not allow anyone to "compute" the private md5. (But who cares....this isn't a bank--the whole system relies on everyone "playing nice" to help us get out of this damn pandemic.) 

	1. Users are asked to store both codes on their own computer (Word doc? Text file?) and are also stored in "local storage" in Chrome for convenience.

* Testing the ESP32 BLE name logging

	* In function `gap_event_handler`, the case `ESP_GAP_BLE_SCAN_RESULT_EVT` means the ESP32 found a BLE name and it should be added to a dynamic memory location called `Encounters` for later retrieval. In use as a contact tracer, we'll reject any name that doesn't start with `#C19:`. 

	* For testing, a line in this function, `fake_test_str(tmp);` forces any incoming BLE name into the `#C19:` format, thus allowing it to be logged.  If this line is uncommented, the ESP32 will log all BLE names it sees. An ESP32 image with this line uncommented is included in this repo as [npct_log.all.bin](https://github.com/tbensky/npct/blob/master/npct/build/npct_log_all.bin). Using this is kind of fun.  As an example, walking around my local hardware store and then a grocery store with it revealed these BLE names:

	<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/allscan.png></p>

	* I have no idea what these devices are; some even ended in a hex code that matched some health conditions.

	* Leaving it on all night at home reveals an "Oral B toothbrush" advertising (likely from a neighbor; I don't own one).

	<p align="center"><img src=https://github.com/tbensky/npct/blob/master/pics/oralb.png></p>

	* So, the `fake_test_str(tmp);` line should be commented out for dedicated contact tracing (and it is in the binary [file to flash](https://github.com/tbensky/npct/blob/master/npct/build/npct.bin)).

* Name logging algorithm

	* For storing each contact, 22 hex-digits are needed (for simplicity, the 22 hex-digits are literally stored in the ESP32 memory): 16 for the ID + 4 for the health code + 2 for the occurrence count.

	* The logging algorithm is a circular buffer that starts kicking out the oldest name at when 5,000 are stored (seems like one can safely `malloc` 110,000 bytes on the stock ESP32 partition, and 110,000/20 = 5,000 possible names stored. 

	* It maintains a count for repeated incoming names, instead of storing another copy of the same name.  Counts max out at 255 (8-bits, or two hex digits).

	* It won't allow the same name to be logged (or counted) successively.  *Some other* name must come in first.


* BLE name, server, characteristic.

	* The BLE name is set via `esp_ble_gap_set_device_name();` at various places inside of the code. Name changes to the device are stored in NVS (non-volatile storage) and are retrieved and set to the BLE name on any startup (device reset or power up).

	* The GATT service UUID is set to 0x00FEED ("feed" in [hexspeak](https://en.wikipedia.org/wiki/Hexspeak)). 

	* Likewise the service's characteristic is 0x00C0DE ("code" in hexspeak).

	* That said, the actual service UUID appears to be `0000feed-0000-1000-8000-00805f9b34fb`.

	* The actual characteristic UUID appears to be `0000c0de-0000-1000-8000-00805f9b34fb`.

	* These longer UUID's are somehow wrapped into the values chosen for `adv_service_uuid128[32]`, but I never understood the full connection. I think 0x00FEED and 0x0C0DE are shorthand for the longer ones.

	



