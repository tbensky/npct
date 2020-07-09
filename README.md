# Contact Tracing

Contact tracing is about you going out and about, and doing two things: (1) letting people you come in close proximity to know about your health condition and (2) likewise you knowing the health condition of those same people. If you are feeling sick, others would benefit from knowing and perhaps acting accordingly if they get sick a few days later.  Likewise, if you are around people who are sick, you might take appropriate actions if you start to feel sick.

So how do we do (1) and (2)?  Do we walk into a grocery store and yell out "I'm not feeling well...if you get sick in a couple of days, it might be because of me!" No, but likely using some technology to quietly inform those around us about our condition would work.

## Mobile Phones for Contact Tracing?

Mobile phones could be ideal for this. "Everyone already has one," and they can broadcast short Bluetooth messages about your health to those (phones) nearby for easy and automated tracking by all later on. 

Well, by Summer 2020, it was becoming clear that phone-based contact tracers were getting kind of stuck. Tons of contact-tracing Apps were appearing, some by companies and individuals; others by countries themselves. Whose App do I use? Am I really doing anything? How do I register? Are they all compatible with each other? And most importantly: Who will have access to my (health) information? 

Open-source Apps revealed all kinds of tricks to keep the Apps running and tracing [in the background on a phone](https://github.com/NHSX).  Sounds good, but battery life was decreacing and trust in Apps and what companies do with your data is always in question.  The latest round is the clipboard spying in stories such as [this one](https://www.computing.co.uk/news/4017082/tiktok-spying-clipboard-researchers-warn-iphone-users) and [this one](https://www.forbes.com/sites/daveywinder/2020/07/04/apple-ios-14-catches-microsofts-linkedin-spying-on-clipboard-tiktok-apps-privacy-iphone-ipad-macbook/#ecac5085896e) didn't help. Phones and Apps are for fun and communication, but we will likely not trust them with health information. (If you want full privacy, you'd have to leave your phone off and at home. The "bag trick" [here](https://youtu.be/s3poKUuvtyM?t=55) would actually works, BTW.)



# The *No Phone* Contact Tracer (NPCT) Project

## Goal of this work

The goal of this work is to provide a totally anonymous contract tracing system that *doesn't use a mobile phone at all.*  Say hello to the "NPCT" (**n**o **p**hone **c**ontact **t**racer) Project.

## Bluetooth

Bluetooth (BT) seems to be an ideal technology for contact tracing. It's a short-range protocol that can broadcast messages to receiving devices nearby. But I didn't want to to use a phone for this, so what's the plan?

The maker movement continues to impress. They got a quick start in backfilling PPE shortages with a mask-making effort, like this one [masks](https://www.makethemasks.com) and [face shields](https://www.prusaprinters.org/prints/25857-prusa-face-shield). So maybe some 'maker' themed contract tracer would work?  

$8....$8. A fully BT-enabled [ESP32](https://esp32.com) is $8 on eBay. Makers use these for all kinds of things, and $8 is about what materials cost for masks people are making.  There had to be some way of using an $8 device for a contact tracer.  So here's my plan:

* Come up with some encoding for a unique ID for yourself + your health information (Easy: an MD5 + some additional hex codes to describe your health).

* Come up with a way of broadcasting your information to those who come near you.  (Easy again: put this encoding into the *BT name* of the ESP32, and tell it to constantly advertise itself.)  By putting it into the BT name, devices don't have to pair to exchange info.

* Come up with a way of monitoring the health of those nearby.  (Easy: Tell the ESP32 to constantly discover advertising devices, and log ones that have a BT name that also encodes one's health info.) 

* Make it easy for one to set/change their health status, and to retrieve a log of other's health info the device found.

* Power it with a $10 portable "extra phone charge" battery.

That's it. It works, and here's how. Want to build one and try it out?

## So participants will need an ESP32 (and to carry one around)?

Yes, and I understand this is a show stopper.  But the things are only $8 and people are making all kinds of masks. Maybe it'll be fun to make and operate your own contact tracer.  Plus, there's a global pandemic out there. Let's help stop it? Also, there is some precedence for a (non-phone) [hardware device for contact tracing](https://simmel.betrusted.io). It's called a "contact tracing hardware token."

# How to build an ESP32 contact tracer

To build the contact tracer, you'll need three items: an [ESP32](https://www.ebay.com/itm/ESP32-ESP-32S-NodeMCU-Development-Board-2-4GHz-WiFi-Bluetooth-Dual-Mode-CP2102/382601606021?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649), a [micro-USB cable](https://www.amazon.com/gp/product/B07PFZDQP6/ref=ppx_yo_dt_b_asin_image_o00_s00?ie=UTF8&psc=1) (most of the time these come with the battery), and a [battery power pack](https://www.amazon.com/gp/product/B00MWU1GGI/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1). (The ones used here are pointed to by the links.)




# Flash on the software






