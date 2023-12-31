= About =

JSLint Plugin for Notepad++ allows you to run JSLint (The JavaScript Code 
Quality Tool) against your open JavaScript files.

Website: http://jslintnpp.sourceforge.net
JSLint Plugin for Notepad++ Copyright 2010 Martin Vladic.

Based on JSLint by Douglas Crockford (http://www.jslint.com/).
JSLint Copyright 2002 Douglas Crockford.

= Release Notes =

Tested and works with Notepad++ version 5.7+; it also may work with older
versions.

= Installation =

Copy JSLintNpp.dll (or JSLintNppA.dll if you are using ANSI version) into your
Notepad++ Plugins folder. You will need to re-start Notepad++ afterward.

= Getting Started = 

1. Open your JavaScript file in Notepad++
2. To JSLint your file select menu option 
   "Plugins - JSLint - JSLint Current File" or press shortcut key Ctrl+Shift+F5.
3. All errors (if such exists) that are found in your file will be displayed in
   list in dockable window at the bottom of Notepad++ main window; document is
   automatically scrolled at the position of the first found error.
4. Double-click on errors in the list to view them inside document; or you can
   press Ctrl+Shift+F8 repeatedly to view errors.

You can play with different JSLint options by choosing 
menu option "Plugins - JSLint - JSLint Options".

= Files =

README.txt     - This file
JSLintNpp.dll  - Unicode version of plugin
JSLintNppA.dll - ANSI version of plugin

= History =

0.8.2
    - JSHint download URL changed to
      https://raw.github.com/jshint/jshint/master/dist/jshint.js
      (let's hope this doesn't change in the future).
    - Detection of JSHint version number added. It is read from the first line
      of source file (let's hope this doesn't change in the future, too).
    - JSLint script updated to version from 2013-11-23
    - JSHint script updated to version 2.3.0

0.8.1
    - JSLint script updated to version from 2012-11-17
    - JSHint script updated to version downloaded from www.jshint.com on 2012-11-22
    - Bugs fixed:
      - "Cannot update to latest jshint (download link returns 404)"
      - "Invalid jshint url in about dialog"

0.8.0
    - JSHint is now supported (choose between original JSLint and JSHint
      in JSLint Options dialog).
    - JSLint script updated to version from 2012-04-15

0.7.4
    - JSLint script updated to version from 2012-03-15
    - Bug fixed:
      "Editor caret is lost when the code has no errors/warnings - ID: 3507651"

0.7.3
    - JSLint script updated to version from 2012-01-29

0.7.2
    - Bug fixed: JSLint plugin crashes in presence of WebEdit plugin
    - JSLint script updated to version from 2011-12-21

0.7.1
    - Bug fixed: version 0.7.0 doesn't read JSLint options from version 0.6.0

0.7.0
    - JSLint plugin now reports unused variables. ("The variables that are
      declared in the function that are not used. This may be an indication of
      an error.")
    - JSLint script updated to version from 2011-10-11

0.6.0
    - V8 JavaScript engine replaced WScript.exe, meaning *major* performance boost
    - JSLint script updated to version from 2011-08-15
    - JSLint plugin is now able to handle HTML text and inspect CSS files
    - Changes in JSLint Options dialog to reflect changes in updated JSLint script
    - New Settings dialog (choose menu option Plugins - JSLint - Settings); here you
      can download latest version of JSLint to experiment with newer version of JSLint, 
      before plugin is updated.

0.5.0   
    - JSLint script updated to version from 2011-04-28

0.4.0   
    - JSLint script updated to version from 2011-01-28
    - Added context menu in list control
    - Easily add predefined globals (Ticket ID 3159082)
    - Bug Fixes

0.3.1   
    - Bug fixes
    - List control now supports multiple selection (use Ctrl+A to select all 
      lints)
    - Added clipboard copy for the selected lints (use Ctrl+C)

0.3.0   
    - New option "Predefined (, separated)" added in Options dialog; this 
      option should be used to specify the names of predefined global variables
    - Added toolbar button that opens Options dialog
    - JSLint script updated to version from 2010-12-23

0.2.1
    - Configuration is now saved in AppData directory (or in whatever
      directory is returned by NPPM_GETPLUGINSCONFIGDIR message)

0.2.0
    - Memory leaks fixed
    - Toolbar added to the JSLint dockable window
    - Visibility state of the JSLint dockable window is now preserved after 
      restart
    - Shortcuts changed (to avoid clashes with existing shortcuts of Notepad++)

0.1.1
    - Bug fixes

0.1.0
    - Initial release
