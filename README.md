obs-websocket
==============

WebSockets API for OBS Studio.

Follow the main author on Twitter for news & updates : [@LePalakis](https://twitter.com/LePalakis)

[![Build Status](https://dev.azure.com/Palakis/obs-websocket/_apis/build/status/Palakis.obs-websocket?branchName=4.x-current)](https://dev.azure.com/Palakis/obs-websocket/_build/latest?definitionId=2&branchName=4.x-current)

## Downloads

Binaries for Windows, MacOS, and Linux are available in the [Releases](https://github.com/Palakis/obs-websocket/releases) section.

## Using obs-websocket

A web client and frontend made by [t2t2](https://github.com/t2t2/obs-tablet-remote) (compatible with tablets and other touch interfaces) is available here : http://t2t2.github.io/obs-tablet-remote/

It is **highly recommended** to protect obs-websocket with a password against unauthorized control. To do this, open the "Websocket server settings" dialog under OBS' "Tools" menu. In the settings dialogs, you can enable or disable authentication and set a password for it.

### Possible use cases

- Remote control OBS from a phone or tablet on the same local network
- Change your stream overlay/graphics based on the current scene
- Automate scene switching with a third-party program (e.g. : auto-pilot, foot pedal, ...)

### For developers

The server is a typical Websockets server running by default on port 4444 (the port number can be changed in the Settings dialog).
The protocol understood by the server is documented in [PROTOCOL.md](docs/generated/protocol.md).

Here's a list of available language APIs for obs-websocket :
- Javascript (browser & nodejs): [obs-websocket-js](https://github.com/haganbmj/obs-websocket-js) by Brendan Hagan
- C#/VB.NET: [obs-websocket-dotnet](https://github.com/Palakis/obs-websocket-dotnet)
- Python 2 and 3: [obs-websocket-py](https://github.com/Elektordi/obs-websocket-py) by Guillaume Genty a.k.a Elektordi
- Python 3.5+ with asyncio: [obs-ws-rc](https://github.com/KirillMysnik/obs-ws-rc) by Kirill Mysnik
- Python 3.6+ with asyncio: [simpleobsws](https://github.com/IRLToolkit/simpleobsws) by tt2468 
- Java 8+: [obs-websocket-java](https://github.com/Twasi/websocket-obs-java) by TwasiNET
- Golang: [go-obs-websocket](https://github.com/christopher-dG/go-obs-websocket) by Chris de Graaf
- HTTP API: [obs-websocket-http](https://github.com/IRLToolkit/obs-websocket-http) by tt2468

I'd like to know what you're building with or for obs-websocket. If you do something in this fashion, feel free to drop me an email at `stephane /dot/ lepin /at/ gmail /dot/ com` !

## Compiling obs-websocket

See the [build instructions](BUILDING.md).

## Contributing

### Branches

Development happens on `4.x-current`

### Pull Requests

Pull Requests must never be based off your fork's main branch (in this case, `4.x-current`). Start your work in a new branch
based on the main one (e.g.: `cool-new-feature`, `fix-palakis-mistakes`, ...) and open a Pull Request once you feel ready to show your work.

**If your Pull Request is not ready to merge yet, create it as a Draft Pull Request** (open the little arrow menu next to the "Create pull request" button, then select "Create draft pull request").

### Code style & formatting

Source code is indented with tabs, with spaces allowed for alignment.

Regarding protocol changes: new and updated request types / events must always come with accompanying documentation comments (see existing protocol elements for examples).
These are required to automatically generate the [protocol specification document](docs/generated/protocol.md).

Among other recommendations: favor return-early code and avoid wrapping huge portions of code in conditionals. As an example, this:

```cpp
if (success) {
    return req->SendOKResponse();
} else {
    return req->SendErrorResponse("something went wrong");
}
```

is better like this:

```cpp
if (!success) {
    return req->SendErrorResponse("something went wrong");
}
return req->SendOKResponse();
```


## Translations

**Your help is welcome on translations**. Please join the localization project on Crowdin: https://crowdin.com/project/obs-websocket

## Special thanks

In (almost) order of appearance:

- [Brendan H.](https://github.com/haganbmj): Code contributions and gooder English in the Protocol specification
- [Mikhail Swift](https://github.com/mikhailswift): Code contributions
- [Tobias Frahmer](https://github.com/Frahmer): Initial German localization
- [Genture](https://github.com/Genteure): Initial Simplified Chinese and Traditional Chinese localizations
- [Larissa Gabilan](https://github.com/laris151): Initial Portuguese localization
- [Andy Asquelt](https://github.com/asquelt): Initial Polish localization
- [Marcel Haazen](https://github.com/nekocentral): Initial Dutch localization
- [Peter Antonvich](https://github.com/pantonvich): Code contributions
- [yinzara](https://github.com/yinzara): Code contributions
- [Chris Angelico](https://github.com/Rosuav): Code contributions
- [Guillaume "Elektordi" Genty](https://github.com/Elektordi): Code contributions
- [Marwin M](https://github.com/dragonbane0): Code contributions
- [Logan S.](https://github.com/lsdaniel): Code contributions
- [RainbowEK](https://github.com/RainbowEK): Code contributions
- [RytoEX](https://github.com/RytoEX): CI script and code contributions
- [Theodore Stoddard](https://github.com/TStod): Code contributions
- [Philip Loche](https://github.com/PicoCentauri): Code contributions
- [Patrick Heyer](https://github.com/PatTheMav): Code contributions and CI fixes
- [Alex Van Camp](https://github.com/Lange): Code contributions
- [Freddie Meyer](https://github.com/DungFu): Code contributions
- [Casey Muller](https://github.com/caseymrm): CI fixes
- [Chris Angelico](https://github.com/Rosuav): Documentation fixes

And also: special thanks to supporters of the project!!

## Supporters

They have contributed financially to the project and made possible the addition of several features into obs-websocket. Many thanks to them!

---

[Support Class](http://supportclass.net) designs and develops professional livestreams, with services ranging from broadcast graphics design and integration to event organization, along many other skills.  

[![Support Class](.github/images/supportclass_logo_blacktext.png)](http://supportclass.net)

---

[MediaUnit](http://www.mediaunit.no) is a Norwegian media company developing products and services for the media industry, primarly focused on web and events.  

[![MediaUnit](.github/images/mediaunit_logo_black.png)](http://www.mediaunit.no/)
