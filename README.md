# Contact Tracing

Contact tracing is about you going out and about, and doing two things: (1) letting people you come in close proximity to know about your health condition and (2) likewise you knowing the health condition of those same people. If you are feeling sick, others would benefit from knowing and perhaps acting accordingly if they get sick a few days later.  Likewise, if you are around people who are sick, you might take appropriate actions if you start to feel sick.

So how do we do (1) and (2)?  Do we walk into a grocery store and yell out "I'm not feeling well...if you get sick in a couple of days, it might be because of me!" No, but likely using some technology to quietly inform those around us about our condition would work.

Mobile phones could be ideal for this. "Everyone already has one," and they can broadcast short Bluetooth messages about your health to those (phones) nearby for easy and automated tracking by all later on. Well, by Summer 2020, it was clear phone-based contact tracers weren't going to work. Trust in Apps and what companies do with your data are always in question. Also, App-based contact tracing approaches are too varied and confusing. Whose App do I use? Am I really doing anything? How do I register?  Why are the Apps different? And, the big question: who will have access to my (health) information? 

Also, although a phone as a device is pretty secure, Apps, privacy, and what companies do with your data is kind of a joke.  The latest round is the clipboard spying in stories such as [this one](https://www.computing.co.uk/news/4017082/tiktok-spying-clipboard-researchers-warn-iphone-users) and [this one](https://www.forbes.com/sites/daveywinder/2020/07/04/apple-ios-14-catches-microsofts-linkedin-spying-on-clipboard-tiktok-apps-privacy-iphone-ipad-macbook/#ecac5085896e) didn't help. Phones and Apps are for fun and communication, but we will likely not trust them with health information. (If you want full privacy, you'd have to leave your phone off and at home. The "bag trick" [here](https://youtu.be/s3poKUuvtyM?t=55) would actually works, BTW.)

By summer 2020, the Corona virus situation was really getting to me. The news was bad and getting worse.  Would we ever emerge from this?  I wanted to "make"  something--anything---that could help us through the Covid-19 crisis. Contact tracing can help, so I thought I'd work on that.

So I made a totally anonymous contract tracing system that doesn't use a mobile phone at all. Say hello to the "NPCT" (**n**o **p**hone **c**ontact **t**racer) Project.

# The No Phone Contact Tracer (NPCT)

I think Bluetooth (BT) is ideal technology for contact tracing. It's a short-range protocol that can broadcast messages to receiving devices nearby. But I didn't want to to use a phone for this, so what's the plan?

I continue to be super impressed with the makers' effort in making [masks](https://www.makethemasks.com) and [face shields](https://www.prusaprinters.org/prints/25857-prusa-face-shield). So maybe some 'maker' themed contract tracer would work?  

$8....$8. A fully BT-enabled [ESP32](https://esp32.com) is $8 on eBay. Makers use these for all kinds of things, and $8 is about what materials cost for masks people are making.  There had to be some way of using an $8 device for a contact tracer.  So here's my plan:

* Come up with some encoding for a unique ID for yourself + your health information (Easy: an MD5 + some additional hex codes to describe your health).

* Come up with a way of broadcasting your information to those who come near you.  (Easy again: put this encoding into the *BT name* of the ESP32, and tell it to constantly advertise itself.)  By putting it into the BT name, devices don't have to pair to exchange info.

* Come up with a way of monitoring the health of those nearby.  (Easy: Tell the ESP32 to constantly discover advertising devices, and log ones that have a BT name that also encodes one's health info.) 

* Make it easy for one to set/change their health status, and to retrieve a log of other's health info the device found.

That's it. It works, and here's how I did it all. Want to build one and try it out?

# So participants will need an ESP32 (and to carry one around)?

Yes, and I understand this is a show stopper.  But the things are only $8 and people are making all kinds of masks. Maybe they'll make this too? Plus, there's a global pandemic out there. Let's help stop it?


# How to build an ESP32 contact tracer

# Flash on the software






