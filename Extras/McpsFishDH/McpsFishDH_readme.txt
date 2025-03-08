; ---------------------------------------------------------------------------
; MircryptionSuite - McpsFishDH addon v. 1.00.07 - dh key exchange
;
; see installation instructions at end of this readme.
;
; 06/01/03, mirc script by Dark Raichu, http://mircryption.sourceforge.net
; 12/05/03, Updated to use the new fish.dll which does 1024 bit keys and is *reportedly* safer.
; 12/20/03, Fixing some bugs that were preventing script from working if not in main mirc directory.
; 12/29/03, Added better error reporting.
; 02/02/04, updated to point to official fish site - thanks to author of fish
;           for informing me of its location and for making the fish source code publically available.
; 04/23/04, updated with new 1080 version of fish.dll
;           changed name of script.
; 05/24/04, message about old version was causing harmless annoying mirc error
; 01/18/05, adding dh support for cbc keys
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; IMPORTANT:
; This addon uses the fish.dll which was written by [anonymous]
;  it comes with version 1.25 of fish, but you should go get the LATEST
;  fish.dll from the original fish author here:
;  ----> http://fish.sekure.us
; Grab the standalone fish dh download and copy the fish.dll into the McpsFishDH directory.
; ---------------------------------------------------------------------------

; ---------------------------------------------------------------------------
; this mirc script (not the dll) is based on mirc code originally started by Sinner
;  and then continued by [anonymous]. the original credits section of blow.mrc:
;     "Big thanks go out to nutty for all his help
;      and to Joffi for his additional ideas
;      And even more thanks go out to mpyx, who helped me alot implementing
;      the secure keyXchange feature.
;      _SiNnEr_ 04-10-2003"
;
; Most of the work is done by the Fish.dll, which was written by [anonymous]
;  and uses code from the MIRACL C library.  thanks for making fish available to all of us.
; ---------------------------------------------------------------------------

;;---------------------------------------------------------------------------
;; IMPORTANT: [anonymous] has released the source code for fish(!) this is
;;  great news and means that the code can be validated if you have any
;;  question about it's integrity.  Still, USE THIS AT YOUR OWN RISK(!)
;;  ----> http://fish.sekure.us
;;---------------------------------------------------------------------------


;;---------------------------------------------------------------------------
;This addon is being provided to add compatibility between mircryption and
; blow, and to allow easier key exchange than mpgp.  Version 2 of mircryption
; will add it's own builtin secure key exchange that operates similarly.
;;---------------------------------------------------------------------------


;---------------------------------------------------------------------------
Installation:
0. Unload and delete any old version of mircryption_SinnerFishDH.mrc, this is
   an old version and will interfere with the new version if you unload it
     /unload -rs mircryption_SinnerFishDH.mrc
1. Copy the McpsFishDH directory to your mirc directory
2. From inside mirc do /load -rs McpsFishDH/mcpsFishDH.mrc
3. See the new fish menu available when right clicking in a query window.
;---------------------------------------------------------------------------

;---------------------------------------------------------------------------
Usage:
open a query window with someone, see new item in right click to exchange a key.
;---------------------------------------------------------------------------