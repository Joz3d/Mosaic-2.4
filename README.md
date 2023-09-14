# NCSA Mosaic 2.4 for X

What Mosaic *actually* looked like.

This is the final widely used version of Mosaic for X.  The Andreessen/Bina version.

**tl;dr**: Nobody used the Mosaic 2.7 that is going around.  Everybody used this one.  They don't look, feel, or act the same.

## The Problem

Many distro repositories have a version of Mosaic 2.7 that you can install.  Also widely
available as a flatpak, snap, etc.  Most of these are from [Alan Dipert's version](https://github.com/alandipert/ncsa-mosaic).  No 
disrespect to the good work that Alan and folks did to update version 2.7, but many sites/repos advertise that version as a way to go back in time and *"see what the first popular web browser was like"*.

The problem is that is **not**  what the first popular browser was like!!  Mosaic 2.7 is from 1996... long after Netscape slayed Mosaic.  Nobody used version 2.7 of Mosaic.  The versions that everybody used were **1.2** and **2.0–2.4**

* Moaic 2.4 was released in early 1994
* By the summer of '94, the Windows and Mac versions overtook the X version in feature-set (as they were getting more use [general public started browsing])
* By late '94 a lot of people were switching to Netscape, and Mosaic was dead by early '95<br>

So the version 2.7 going around from almost 2 years after the browser died, updated by a completely different 
dev team, and with a different look and feel, was not a thing.

In the Mosaics that everyone actually used, there was no toolbar at the top!  There was no address bar that you could just type in the URL and hit enter!!  That wasn't invented yet!  There was no security icons (or security!)  There was no back and forward graphical buttons at the top – they were TEXT buttons, and they were at the BOTTOM!  Sheesh!  So, touting version 2.7 as a Mosaic experience is total revisionist history.

## Finding Mosaic 2.4

There are great updates of Mosaics that had ~100% market share in 1993, that being [Mosaic 1.0](https://github.com/lorkki/xmosaic-1.0) and [1.2](https://github.com/csev/xmosaic-1.2/)

I could not find the source for a 2.0–2.4 version until I came across [Udo Munk's video](https://www.youtube.com/watch?v=L9seeEJzcE0) and he actually still had it.  So big thanks to [Udo Munk](https://github.com/udo-munk)!

And so here is NCSA Mosaic 2.4 for X – the last widely used version of Mosaic... if you want to *actually* see what the browsing experience looked like with Mosaic 😁

I didn't do much to the source.  I mostly followed [Pedro Vicente's 2.7 build fixes](https://github.com/pedro-vicente/NCSA_Mosaic) and added a few minor updates for newer glibc and the such.

## Building

**Prerequisite packages:**
```
sudo apt install libxt-dev libmotif-dev libxmu-headers libxmu-dev libxpm-dev
```
**Mosaic:**

`make` and the binary spits out in src/, just like back in '94.

**Fix Fonts:**
```
apt install gsfonts-other gsfonts-x11
```
...and restart your X server.


## Sites that work

It's not updated for protocol (yet?), so most sites don't work due to lack of support for HTTP host header and hence no support for name-based virtual hosts.  Until that is fixed, the following list of websites can be browsed:

* [Mosaic Communications Corporation](http://home.mcom.com) - Netscape before they called it Netscape (tx rate limited for a dial-up feel!)
* [Mark Pesce's original home page](http://hyperreal.org/~mpesce) - Asks to save binary but you can cancel out and page loads
* [Nelson H. F. Beebe's Home Page](http://www.math.utah.edu/~beebe) - Same binary prompt as above
* [The Armory](http://www.armory.com/)
* Let me know what else you find that works

#### Other Lost Mosaic Versions

If anyone has source code for xmosaic 0.5–0.10, please let me know.  I would love to see those very early versions.
