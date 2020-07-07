# Contact Tracing

Contact tracing is about you going out and about, and doing two things: (1) letting people you come in close proximity to know about your health condition and (2) likewise knowing the health condition of those same people. If you are feeling sick, others would benefit from knowing and maybe act accordingly if they get sick a few days later.  Likewise, if you are around people who are sick, you might take appropriate actions if you start to feel sick.

So how do we do (1) and (2)?  Do we walk into a grocery store and yell out "I'm not feeling well...if you get sick in a couple of days, it might be because of me!" No.  But how about using some tech. to quietly inform those around us about our condition?

Mobile phones are perhaps ideal for this. "Everyone already has one," and they can broadcast short Bluetooth messages about your health to those nearby for easy and automated tracking by all later on. Well, by Summer 2020, it was clear phone-based contact tracers weren't going to work. No one trusts Apps, and approaches to contract tracing are confusing. Am I really doing anything? How do I register? How? Why are the Apps different, and who will have access to my information? 

Also, although a phone itself may be secure as a device, Apps, privacy, and what they do with your data is kind of a joke.  The latest round is the clipboard spying in stories such as [this one](https://www.computing.co.uk/news/4017082/tiktok-spying-clipboard-researchers-warn-iphone-users) and [this one](https://www.forbes.com/sites/daveywinder/2020/07/04/apple-ios-14-catches-microsofts-linkedin-spying-on-clipboard-tiktok-apps-privacy-iphone-ipad-macbook/#ecac5085896e) didn't help. Phones and Apps are for fun and communication, but no one is going to trust them with health information. (If you want full privacy, leave your phone off and at home. The "bag trick" [here](https://youtu.be/s3poKUuvtyM?t=55) would actually works, BTW.)

By summer 2020, the virus was really getting to me. The news was bad and getting worse.  Would we ever emerge from this?  I wanted do do something--anything---to help us through the Covid-19 crisis. Contact tracing can help, so I thought I'd work on something to help contain the Covid-19 outbreak.

So I made a totally anonymous contract tracing system that doesn't use a mobile phone at all. Say hello to "NPCT," the "no phone contact tracer."

# NPCT: The "no phone" Contact Tracer

I think Bluetooth (BT) is ideal for contact tracing. It's a short-range protocol that can broadcast messages to receiving devices nearby. But you can't use a phone. So what's the plan?

$8....$8. A fully BT-enabled ESP32 is $8 on eBay. This about what materials cost for masks people are making.  There had to be someway of using this for a contact tracer.  So here's my plan:

* Come up with some encoding for a unique ID for yourself + your health information (easy: an MD5 + some additional hex codes to describe your health).
* Put this encoding into the BT name of the ESP32. Tell it to constantly advertise itself. This takes care of broadcasting your information to those who come near you.
* Also tell the ESP32 to constantly discover advertising devices, and log ones that have a BT name that also encodes one's health info. 
* Make it easy for one to set/change their health status, and to retrieve a log of other's health info the device found.

That's it. It works, and here's how I did it all.

# 





