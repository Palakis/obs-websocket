<!-- This file was generated based on handlebars templates. Do not edit directly! -->

# obs-websocket 4.1 protocol reference

**This is the reference for the latest 4.1 development build. [See here for obs-websocket 4.0.0!](https://github.com/Palakis/obs-websocket/blob/4.0.0/PROTOCOL.md)**

# General Introduction
Messages are exchanged between the client and the server as JSON objects.
This protocol is based on the original OBS Remote protocol created by Bill Hamilton, with new commands specific to OBS Studio.


# Authentication
OBSWebSocket uses SHA256 to transmit credentials.

A request for [`GetAuthRequired`](#getauthrequired) returns two elements:
- A `challenge`: a random string that will be used to generate the auth response.
- A `salt`: applied to the password when generating the auth response.

To generate the answer to the auth challenge, follow this procedure:
- Concatenate the user declared password with the `salt` sent by the server (in this order: `password + server salt`).
- Generate a binary SHA256 hash of the result and encode the resulting SHA256 binary hash to base64, known as a `base64 secret`.
- Concatenate the base64 secret with the `challenge` sent by the server (in this order: `base64 secret + server challenge`).
- Generate a binary SHA256 hash of the result and encode it to base64.
- Voilà, this last base64 string is the `auth response`. You may now use it to authenticate to the server with the [`Authenticate`](#authenticate) request.

Pseudo Code Example:
```
password = "supersecretpassword"
challenge = "ztTBnnuqrqaKDzRM3xcVdbYm"
salt = "PZVbYpvAnZut2SS6JNJytDm9"

secret_string = password + salt
secret_hash = binary_sha256(secret_string)
secret = base64_encode(secret_hash)

auth_response_string = secret + challenge
auth_response_hash = binary_sha256(auth_response_string)
auth_response = base64_encode(auth_response_hash)
```




# Table of Contents

<!-- toc -->

- [Events](#events)
  * [Scenes](#scenes)
    + [OnSceneChange](#onscenechange)
    + [ScenesChanged](#sceneschanged)
    + [SceneCollectionChanged](#scenecollectionchanged)
    + [SceneCollectionListChanged](#scenecollectionlistchanged)
  * [Transitions](#transitions)
    + [SwitchTransition](#switchtransition)
    + [TransitionListChanged](#transitionlistchanged)
    + [TransitionDurationChanged](#transitiondurationchanged)
    + [TransitionBegin](#transitionbegin)
  * [Profiles](#profiles)
    + [ProfileChanged](#profilechanged)
    + [ProfileListChanged](#profilelistchanged)
  * [Streaming](#streaming)
    + [StreamStarting](#streamstarting)
    + [StreamStarted](#streamstarted)
    + [StreamStopping](#streamstopping)
    + [StreamStopped](#streamstopped)
    + [StreamStatus](#streamstatus)
  * [Recording](#recording)
    + [RecordingStarting](#recordingstarting)
    + [RecordingStarted](#recordingstarted)
    + [RecordingStopping](#recordingstopping)
    + [RecordingStopped](#recordingstopped)
  * [Other](#other)
    + [Exiting](#exiting)
  * [Sources](#sources)
    + [SourceOrderChanged](#sourceorderchanged)
    + [SceneItemAdded](#sceneitemadded)
    + [SceneItemRemoved](#sceneitemremoved)
    + [SceneItemVisibilityChanged](#sceneitemvisibilitychanged)
  * [Studio Mode](#studio-mode)
    + [PreviewSceneChanged](#previewscenechanged)
    + [StudioModeSwitched](#studiomodeswitched)
- [Requests](#requests)
  * [General](#general)
    + [GetVersion](#getversion)
    + [GetAuthRequired](#getauthrequired)
    + [Authenticate](#authenticate)
  * [Scenes](#scenes-1)
    + [SetCurrentScene](#setcurrentscene)
    + [GetCurrentScene](#getcurrentscene)
    + [GetSceneList](#getscenelist)
  * [Sources](#sources-1)
    + [SetSourceRender](#setsourcerender)
    + [SetVolume](#setvolume)
    + [GetVolume](#getvolume)
    + [ToggleMute](#togglemute)
    + [SetMute](#setmute)
    + [GetMute](#getmute)
    + [SetSceneItemPosition](#setsceneitemposition)
    + [SetSceneItemTransform](#setsceneitemtransform)
    + [SetSceneItemCrop](#setsceneitemcrop)
    + [GetTextGDIPlusProperties](#gettextgdiplusproperties)
    + [SetTextGDIPlusProperties](#settextgdiplusproperties)
    + [GetBrowserSourceProperties](#getbrowsersourceproperties)
    + [SetBrowserSourceProperties](#setbrowsersourceproperties)
    + [ResetSceneItem](#resetsceneitem)
  * [Streaming](#streaming-1)
    + [GetStreamingStatus](#getstreamingstatus)
    + [StartStopStreaming](#startstopstreaming)
    + [StartStreaming](#startstreaming)
    + [StopStreaming](#stopstreaming)
  * [Recording](#recording-1)
    + [StartStopRecording](#startstoprecording)
    + [StartRecording](#startrecording)
    + [StopRecording](#stoprecording)
    + [SetRecordingFolder](#setrecordingfolder)
    + [GetRecordingFolder](#getrecordingfolder)
  * [Transitions](#transitions-1)
    + [GetTransitionList](#gettransitionlist)
    + [GetCurrentTransition](#getcurrenttransition)
    + [SetCurrentTransition](#setcurrenttransition)
    + [SetTransitionDuration](#settransitionduration)
    + [GetTransitionDuration](#gettransitionduration)
  * [Scene Collections](#scene-collections)
    + [SetCurrentSceneCollection](#setcurrentscenecollection)
    + [GetCurrentSceneCollection](#getcurrentscenecollection)
  * [Profiles](#profiles-1)
    + [SetCurrentProfile](#setcurrentprofile)
    + [GetCurrentProfile](#getcurrentprofile)
    + [ListProfiles](#listprofiles)
  * [Settings](#settings)
    + [SetStreamingSettings](#setstreamingsettings)
    + [GetStreamSettings](#getstreamsettings)
    + [SaveStreamSettings](#savestreamsettings)
  * [Studio Mode](#studio-mode-1)
    + [GetStudioModeStatus](#getstudiomodestatus)
    + [GetPreviewScene](#getpreviewscene)
    + [SetPreviewScene](#setpreviewscene)
    + [TransitionToProgram](#transitiontoprogram)
    + [EnableStudioMode](#enablestudiomode)
    + [DisableStudioMode](#disablestudiomode)
    + [ToggleStudioMode](#togglestudiomode)
    + [GetSpecialSources](#getspecialsources)

<!-- tocstop -->

# Events
Events are broadcast by the server to each connected client when a recognized action occurs within OBS.

An event message will contain at least the following base fields:
- `update-type` _String_: the type of event.
- `stream-timecode` _String (optional)_: time elapsed between now and stream start (only present if OBS Studio is streaming).
- `rec-timecode` _String (optional)_: time elapsed between now and recording start (only present if OBS Studio is recording).

Timecodes are sent using the format: `HH:MM:SS.mmm`

Additional fields may be present in the event message depending on the event type.


## Scenes

### OnSceneChange

Indicates a scene change.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | The new scene. |
| `sources` | _Array_ | List of sources in the new scene. |


---

### ScenesChanged

The scene list has been modified.
Scenes have been added, removed, or renamed.

**Response Items:**

_No additional response items._

---

### SceneCollectionChanged

Triggered when switching to another scene collection or when renaming the current scene collection.

**Response Items:**

_No additional response items._

---

### SceneCollectionListChanged

Triggered when a scene collection is created, added, renamed, or removed.

**Response Items:**

_No additional response items._

---

## Transitions

### SwitchTransition

The active transition has been changed.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `transition-name` | _String_ | The name of the new active transition. |


---

### TransitionListChanged

The list of available transitions has been modified.
Transitions have been added, removed, or renamed.

**Response Items:**

_No additional response items._

---

### TransitionDurationChanged

The active transition duration has been changed.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `new-duration` | _int_ | New transition duration. |


---

### TransitionBegin

A transition (other than "cut") has begun.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `name` | _String_ | Transition name. |
| `duration` | _int_ | Transition duration (in milliseconds). |


---

## Profiles

### ProfileChanged

Triggered when switching to another profile or when renaming the current profile.

**Response Items:**

_No additional response items._

---

### ProfileListChanged

Triggered when a profile is created, added, renamed, or removed.

**Response Items:**

_No additional response items._

---

## Streaming

### StreamStarting

A request to start streaming has been issued.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `preview-only` | _boolean_ | Always false (retrocompatibility). |


---

### StreamStarted

Streaming started successfully.

**Response Items:**

_No additional response items._

---

### StreamStopping

A request to stop streaming has been issued.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `preview-only` | _boolean_ | Always false (retrocompatibility). |


---

### StreamStopped

Streaming stopped successfully.

**Response Items:**

_No additional response items._

---

### StreamStatus

Emit every 2 seconds.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `streaming` | _boolean_ | Current streaming state. |
| `recording` | _boolean_ | Current recording state. |
| `preview-only` | _boolean_ | Always false (retrocompatibility). |
| `bytes-per-sec` | _int_ | Amount of data per second (in bytes) transmitted by the stream encoder. |
| `kbits-per-sec` | _int_ | Amount of data per second (in kilobits) transmitted by the stream encoder. |
| `strain` | _double_ | Percentage of dropped frames. |
| `total-stream-time` | _int_ | Total time (in seconds) since the stream started. |
| `num-total-frames` | _int_ | Total number of frames transmitted since the stream started. |
| `num-dropped-frames` | _int_ | Number of frames dropped by the encoder since the stream started. |
| `fps` | _double_ | Current framerate. |


---

## Recording

### RecordingStarting

A request to start recording has been issued.

**Response Items:**

_No additional response items._

---

### RecordingStarted

Recording started successfully.

**Response Items:**

_No additional response items._

---

### RecordingStopping

A request to stop recording has been issued.

**Response Items:**

_No additional response items._

---

### RecordingStopped

Recording stopped successfully.

**Response Items:**

_No additional response items._

---

## Other

### Exiting

OBS is exiting.

**Response Items:**

_No additional response items._

---

## Sources

### SourceOrderChanged

Scene items have been reordered.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | Name of the scene where items have been reordered. |


---

### SceneItemAdded

An item has been added to the current scene.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | Name of the scene. |
| `item-name` | _String_ | Name of the item added to the scene. |


---

### SceneItemRemoved

An item has been removed from the current scene.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | Name of the scene. |
| `item-name` | _String_ | Name of the item removed from the scene. |


---

### SceneItemVisibilityChanged

An item's visibility has been toggled.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | Name of the scene. |
| `item-name` | _String_ | Name of the item in the scene. |
| `item-visible` | _boolean_ | New visibility state of the item. |


---

## Studio Mode

### PreviewSceneChanged

The selected preview scene has changed (only available in Studio Mode).

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | Name of the scene being previewed. |
| `sources` | _Source\|Array_ | List of sources composing the scene. Same specification as [`GetCurrentScene`](#getcurrentscene). |


---

### StudioModeSwitched

Studio Mode has been enabled or disabled.

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `new-state` | _boolean_ | The new enabled state of Studio Mode. |


---




# Requests
Requests are sent by the client and require at least the following two fields:
- `request-type` _String_: String name of the request type.
- `message-id` _String_: Client defined identifier for the message, will be echoed in the response.

Once a request is sent, the server will return a JSON response with at least the following fields:
- `message-id` _String_: The client defined identifier specified in the request.
- `status` _String_: Response status, will be one of the following: `ok`, `error`
- `error` _String_: An error message accompanying an `error` status.

Additional information may be required/returned depending on the request type. See below for more information.


## General

### GetVersion

Returns the latest version of the plugin and the API.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `version` | _double_ | OBSRemote compatible API version. Fixed to 1.1 for retrocompatibility. |
| `obs-websocket-version` | _String_ | obs-websocket plugin version. |
| `obs-studio-version` | _String_ | OBS Studio program version. |
| `available-requests` | _String\|Array_ | List of available request types. |


---

### GetAuthRequired

Tells the client if authentication is required. If so, returns authentication parameters `challenge`
and `salt` (see "Authentication" for more information).

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `authRequired` | _boolean_ | Indicates whether authentication is required. |
| `challenge` | _String (optional)_ |  |
| `salt` | _String (optional)_ |  |


---

### Authenticate

Attempt to authenticate the client to the server.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `auth` | _String_ | Response to the auth challenge (see "Authentication" for more information). |


**Response Items:**

_No additional response items._

---

## Scenes

### SetCurrentScene

Switch to the specified scene.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | Name of the scene to switch to. |


**Response Items:**

_No additional response items._

---

### GetCurrentScene

Get the current scene's name and source items.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `name` | _String_ | Name of the currently active scene. |
| `sources` | _Source\|Array_ | Ordered list of the current scene's source items. |


---

### GetSceneList

Get a list of scenes in the currently active profile.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `current-scene` | _String_ | Name of the currently active scene. |
| `scenes` | _Scene\|Array_ | Ordered list of the current profile's scenes (See `[GetCurrentScene](#getcurrentscene)` for more information). |


---

## Sources

### SetSourceRender

Show or hide a specified source item in a specified scene.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `source` | _String_ | Name of the source in the specified scene. |
| `render` | _boolean_ | Desired visibility. |
| `scene-name` | _String (optional)_ | Name of the scene where the source resides. Defaults to the currently active scene. |


**Response Items:**

_No additional response items._

---

### SetVolume

Set the volume of the specified source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `source` | _String_ | Name of the source. |
| `volume` | _double_ | Desired volume. Must be between `0.0` and `1.0`. |


**Response Items:**

_No additional response items._

---

### GetVolume

Get the volume of the specified source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `source` | _String_ | Name of the source. |


**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `name` | _String_ | Name of the source. |
| `volume` | _double_ | Volume of the source. Between `0.0` and `1.0`. |
| `mute` | _boolean_ | Indicates whether the source is muted. |


---

### ToggleMute

Inverts the mute status of a specified source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `source` | _String_ | The name of the source. |


**Response Items:**

_No additional response items._

---

### SetMute

Sets the mute status of a specified source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `source` | _String_ | The name of the source. |
| `mute` | _boolean_ | Desired mute status. |


**Response Items:**

_No additional response items._

---

### GetMute

Get the mute status of a specified source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `source` | _String_ | The name of the source. |


**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `name` | _String_ | The name of the source. |
| `muted` | _boolean_ | Mute status of the source. |


---

### SetSceneItemPosition

Sets the coordinates of a specified source item.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | The name of the scene that the source item belongs to. Defaults to the current scene. |
| `item` | _String_ | The name of the source item. |
| `x` | _double_ | X coordinate. |
| `y` | _double_ | Y coordinate. |


**Response Items:**

_No additional response items._

---

### SetSceneItemTransform

Set the transform of the specified source item.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | The name of the scene that the source item belongs to. Defaults to the current scene. |
| `item` | _String_ | The name of the source item. |
| `x-scale` | _double_ | Width scale factor. |
| `y-scale` | _double_ | Height scale factor. |
| `rotation` | _double_ | Source item rotation (in degrees). |


**Response Items:**

_No additional response items._

---

### SetSceneItemCrop

Sets the crop coordinates of the specified source item.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | the name of the scene that the source item belongs to. Defaults to the current scene. |
| `item` | _String_ | The name of the source. |
| `top` | _int_ | Pixel position of the top of the source item. |
| `bottom` | _int_ | Pixel position of the bottom of the source item. |
| `left` | _int_ | Pixel position of the left of the source item. |
| `right` | _int_ | Pixel position of the right of the source item. |


**Response Items:**

_No additional response items._

---

### GetTextGDIPlusProperties

Get the current properties of a Text GDI Plus source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | Name of the scene to retrieve. Defaults to the current scene. |
| `source` | _String_ | Name of the source. |


**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `align` | _String_ | Text Alignment ("left", "center", "right"). |
| `bk-color` | _int_ | Background color. |
| `bk-opacity` | _int_ | Background opacity (0-100). |
| `chatlog` | _boolean_ | Chat log. |
| `chatlog_lines` | _int_ | Chat log lines. |
| `color` | _int_ | Text color. |
| `extents` | _boolean_ | Extents wrap. |
| `extents_cx` | _int_ | Extents cx. |
| `extents_cy` | _int_ | Extents cy. |
| `file` | _String_ | File path name. |
| `read_from_file` | _boolean_ | Read text from the specified file. |
| `font` | _Object_ | Holds data for the font. Ex: `"font": { "face": "Arial", "flags": 0, "size": 150, "style": "" }` |
| `font.face` | _String_ | Font face. |
| `font.flags` | _int_ | Font text styling flag. `Bold=1, Italic=2, Bold Italic=3, Underline=5, Strikeout=8` |
| `font.size` | _int_ | Font text size. |
| `font.style` | _String_ | Font Style (unknown function). |
| `gradient` | _boolean_ | Gradient enabled. |
| `gradient_color` | _int_ | Gradient color. |
| `gradient_dir` | _float_ | Gradient direction. |
| `gradient_opacity` | _int_ | Gradient opacity (0-100). |
| `outline` | _boolean_ | Outline. |
| `outline_color` | _int_ | Outline color. |
| `outline_size` | _int_ | Outline size. |
| `outline_opacity` | _int_ | Outline opacity (0-100). |
| `text` | _String_ | Text content to be displayed. |
| `valign` | _String_ | Text vertical alignment ("top", "center", "bottom"). |
| `vertical` | _boolean_ | Vertical text enabled. |
| `render` | _boolean_ | Visibility of the scene item. |


---

### SetTextGDIPlusProperties

Get the current properties of a Text GDI Plus source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | Name of the scene to retrieve. Defaults to the current scene. |
| `source` | _String_ | Name of the source. |
| `align` | _String (optional)_ | Text Alignment ("left", "center", "right"). |
| `bk-color` | _int (optional)_ | Background color. |
| `bk-opacity` | _int (optional)_ | Background opacity (0-100). |
| `chatlog` | _boolean (optional)_ | Chat log. |
| `chatlog_lines` | _int (optional)_ | Chat log lines. |
| `color` | _int (optional)_ | Text color. |
| `extents` | _boolean (optional)_ | Extents wrap. |
| `extents_cx` | _int (optional)_ | Extents cx. |
| `extents_cy` | _int (optional)_ | Extents cy. |
| `file` | _String (optional)_ | File path name. |
| `read_from_file` | _boolean (optional)_ | Read text from the specified file. |
| `font` | _Object (optional)_ | Holds data for the font. Ex: `"font": { "face": "Arial", "flags": 0, "size": 150, "style": "" }` |
| `font.face` | _String (optional)_ | Font face. |
| `font.flags` | _int (optional)_ | Font text styling flag. `Bold=1, Italic=2, Bold Italic=3, Underline=5, Strikeout=8` |
| `font.size` | _int (optional)_ | Font text size. |
| `font.style` | _String (optional)_ | Font Style (unknown function). |
| `gradient` | _boolean (optional)_ | Gradient enabled. |
| `gradient_color` | _int (optional)_ | Gradient color. |
| `gradient_dir` | _float (optional)_ | Gradient direction. |
| `gradient_opacity` | _int (optional)_ | Gradient opacity (0-100). |
| `outline` | _boolean (optional)_ | Outline. |
| `outline_color` | _int (optional)_ | Outline color. |
| `outline_size` | _int (optional)_ | Outline size. |
| `outline_opacity` | _int (optional)_ | Outline opacity (0-100). |
| `text` | _String (optional)_ | Text content to be displayed. |
| `valign` | _String (optional)_ | Text vertical alignment ("top", "center", "bottom"). |
| `vertical` | _boolean (optional)_ | Vertical text enabled. |
| `render` | _boolean (optional)_ | Visibility of the scene item. |


**Response Items:**

_No additional response items._

---

### GetBrowserSourceProperties

Get current properties for a Browser Source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | Name of the scene that the source belongs to. Defaults to the current scene. |
| `source` | _String_ | Name of the source. |


**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `is_local_file` | _boolean_ | Indicates that a local file is in use. |
| `url` | _String_ | Url or file path. |
| `css` | _String_ | CSS to inject. |
| `width` | _int_ | Width. |
| `height` | _int_ | Height. |
| `fps` | _int_ | Framerate. |
| `shutdown` | _boolean_ | Indicates whether the source should be shutdown when not visible. |
| `render` | _boolean (optional)_ | Visibility of the scene item. |


---

### SetBrowserSourceProperties

Set current properties for a Browser Source.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | Name of the scene that the source belongs to. Defaults to the current scene. |
| `source` | _String_ | Name of the source. |
| `is_local_file` | _boolean (optional)_ | Indicates that a local file is in use. |
| `url` | _String (optional)_ | Url or file path. |
| `css` | _String (optional)_ | CSS to inject. |
| `width` | _int (optional)_ | Width. |
| `height` | _int (optional)_ | Height. |
| `fps` | _int (optional)_ | Framerate. |
| `shutdown` | _boolean (optional)_ | Indicates whether the source should be shutdown when not visible. |
| `render` | _boolean (optional)_ | Visibility of the scene item. |


**Response Items:**

_No additional response items._

---

### ResetSceneItem

Reset a source item.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String (optional)_ | Name of the scene the source belogns to. Defaults to the current scene. |
| `item` | _String_ | Name of the source item. |


**Response Items:**

_No additional response items._

---

## Streaming

### GetStreamingStatus

Get current streaming and recording status.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `streaming` | _boolean_ | Current streaming status. |
| `recording` | _boolean_ | Current recording status. |
| `stream-timecode` | _String (optional)_ | Time elapsed since streaming started (only present if currently streaming). |
| `rec-timecode` | _String (optional)_ | Time elapsed since recording started (only present if currently recording). |
| `preview-only` | _boolean_ | Always false. Retrocompatibility with OBSRemote. |


---

### StartStopStreaming

Toggle streaming on or off.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### StartStreaming

Start streaming.
Will return an `error` if streaming is already active.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `stream` | _Object (optional)_ | Special stream configuration. |
| `type` | _String (optional)_ | If specified ensures the type of stream matches the given type (usually 'rtmp_custom' or 'rtmp_common'). If the currently configured stream type does not match the given stream type, all settings must be specified in the `settings` object or an error will occur when starting the stream. |
| `metadata` | _Object (optional)_ | Adds the given object parameters as encoded query string parameters to the 'key' of the RTMP stream. Used to pass data to the RTMP service about the streaming. May be any String, Numeric, or Boolean field. |
| `settings` | _Object (optional)_ | Settings for the stream. |
| `settings.server` | _String (optional)_ | The publish URL. |
| `settings.key` | _String (optional)_ | The publish key of the stream. |
| `settings.use-auth` | _boolean (optional)_ | Indicates whether authentication should be used when connecting to the streaming server. |
| `settings.username` | _String (optional)_ | If authentication is enabled, the username for the streaming server. Ignored if `use-auth` is not set to `true`. |
| `settings.password` | _String (optional)_ | If authentication is enabled, the password for the streaming server. Ignored if `use-auth` is not set to `true`. |


**Response Items:**

_No additional response items._

---

### StopStreaming

Stop streaming.
Will return an `error` if streaming is not active.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

## Recording

### StartStopRecording

Toggle recording on or off.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### StartRecording

Start recording.
Will return an `error` if recording is already active.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### StopRecording

Stop recording.
Will return an `error` if recording is not active.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### SetRecordingFolder

Change the current recording folder.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `rec-folder` | _Stsring_ | Path of the recording folder. |


**Response Items:**

_No additional response items._

---

### GetRecordingFolder

Get the path of  the current recording folder.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `rec-folder` | _Stsring_ | Path of the recording folder. |


---

## Transitions

### GetTransitionList

List of all transitions available in the frontend's dropdown menu.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `current-transition` | _String_ | Name of the currently active transition. |
| `transitions` | _Object\|Array_ | List of transitions. |
| `transitions[].name` | _String_ | Name of the transition. |


---

### GetCurrentTransition

Get the name of the currently selected transition in the frontend's dropdown menu.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `name` | _String_ | Name of the selected transition. |
| `duration` | _int (optional)_ | Transition duration (in milliseconds) if supported by the transition. |


---

### SetCurrentTransition

Set the active transition.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `transition-name` | _String_ | The name of the transition. |


**Response Items:**

_No additional response items._

---

### SetTransitionDuration

Set the duration of the currently selected transition if supported.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `duration` | _int_ | Desired duration of the transition (in milliseconds). |


**Response Items:**

_No additional response items._

---

### GetTransitionDuration

Get the duration of the currently selected transition if supported.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `transition-duration` | _int_ | Duration of the current transition (in milliseconds). |


---

## Scene Collections

### SetCurrentSceneCollection

Change the active scene collection.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `sc-name` | _String_ | Name of the desired scene collection. |


**Response Items:**

_No additional response items._

---

### GetCurrentSceneCollection

Get the name of the current scene collection.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `sc-name` | _String_ | Name of the currently active scene collection. |


---

## Profiles

### SetCurrentProfile

Set the currently active profile.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `profile-name` | _String_ | Name of the desired profile. |


**Response Items:**

_No additional response items._

---

### GetCurrentProfile

Get the name of the current profile.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `profile-name` | _String_ | Name of the currently active profile. |


---

### ListProfiles

Get a list of available profiles.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `profiles` | _Object\|Array_ | List of available profiles. |


---

## Settings

### SetStreamingSettings

Sets one or more attributes of the current streaming server settings. Any options not passed will remain unchanged. Returns the updated settings in response. If 'type' is different than the current streaming service type, all settings are required. Returns the full settings of the stream (the same as GetStreamSettings).

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `type` | _String_ | The type of streaming service configuration, usually `rtmp_custom` or `rtmp_common`. |
| `settings` | _Object_ | The actual settings of the stream. |
| `settings.server` | _String (optional)_ | The publish URL. |
| `settings.key` | _String (optional)_ | The publish key. |
| `settings.use-auth` | _boolean (optional)_ | Indicates whether authentication should be used when connecting to the streaming server. |
| `settings.username` | _String (optional)_ | The username for the streaming service. |
| `settings.password` | _String (optional)_ | The password for the streaming service. |
| `save` | _boolean_ | Persist the settings to disk. |


**Response Items:**

_No additional response items._

---

### GetStreamSettings

Get the current streaming server settings.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `type` | _String_ | The type of streaming service configuration. Usually 'rtmp_custom' or 'rtmp_common'. |
| `settings` | _Object_ | Setings of the stream. |
| `settings.server` | _String_ | The publish URL. |
| `settings.key` | _String_ | The publish key of the stream. |
| `settings.use-auth` | _boolean_ | Indicates whether audentication should be used when connecting to the streaming server. |
| `settings.username` | _String_ | The username to use when accessing the streaming server. Only present if `use-auth` is `true`. |
| `settings.password` | _String_ | The password to use when accessing the streaming server. Only present if `use-auth` is `true`. |


---

### SaveStreamSettings

Save the current streaming server settings to disk.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

## Studio Mode

### GetStudioModeStatus

Indicates if Studio Mode is currently enabled.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `studio-mode` | _boolean_ | Indicates if Studio Mode is enabled. |


---

### GetPreviewScene

Get the name of the currently previewed scene and its list of sources.
Will return an `error` if Studio Mode is not enabled.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `name` | _String_ | The name of the active preview scene. |
| `sources` | _Source\|Array_ |  |


---

### SetPreviewScene

Set the active preview scene.
Will return an `error` if Studio Mode is not enabled.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `scene-name` | _String_ | The name of the scene to preview. |


**Response Items:**

_No additional response items._

---

### TransitionToProgram

Transitions the currently previewed scene to the main output.
Will return an `error` if Studio Mode is not enabled.

**Request Fields:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `with-transition` | _Object (optional)_ | Change the active transition before switching scenes. Defaults to the active transition. |
| `with-transition.name` | _String_ | Name of the transition. |
| `with-transition.duration` | _int (optional)_ | Transition duration (in milliseconds). |


**Response Items:**

_No additional response items._

---

### EnableStudioMode

Enables Studio Mode.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### DisableStudioMode

Disables Studio Mode.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### ToggleStudioMode

Toggles Studio Mode.

**Request Fields:**

_No specified parameters._

**Response Items:**

_No additional response items._

---

### GetSpecialSources

Get configured special sources like Desktop Audio and Mic/Aux sources.

**Request Fields:**

_No specified parameters._

**Response Items:**

| Name | Type  | Description |
| ---- | :---: | ------------|
| `desktop-1` | _String (optional)_ | Name of the first Desktop Audio capture source. |
| `desktop-2` | _String (optional)_ | Name of the second Desktop Audio capture source. |
| `mic-1` | _String (optional)_ | Name of the first Mic/Aux input source. |
| `mic-2` | _String (optional)_ | Name of the second Mic/Aux input source. |
| `mic-3` | _String (optional)_ | NAme of the third Mic/Aux input source. |


---


