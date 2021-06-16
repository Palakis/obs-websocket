#include <obs.hpp>
#include <obs-frontend-api.h>

#include "Utils.h"

#include "../plugin-macros.generated.h"

#define CASE(x) case x: return #x;

std::vector<std::string> ConvertStringArray(char **array)
{
	std::vector<std::string> ret;
	if (!array)
		return ret;

	size_t index = 0;
	char* value = nullptr;
	do {
		value = array[index];
		if (value)
			ret.push_back(value);
		index++;
	} while (value);

	return ret;
}

std::string Utils::Obs::StringHelper::GetObsVersionString()
{
	uint32_t version = obs_get_version();

	uint8_t major, minor, patch;
	major = (version >> 24) & 0xFF;
	minor = (version >> 16) & 0xFF;
	patch = version & 0xFF;

	QString combined = QString("%1.%2.%3").arg(major).arg(minor).arg(patch);
	return combined.toStdString();
}

std::string Utils::Obs::StringHelper::GetCurrentSceneCollection()
{
	char *sceneCollectionName = obs_frontend_get_current_scene_collection();
	std::string ret = sceneCollectionName;
	bfree(sceneCollectionName);
	return ret;
}

std::string Utils::Obs::StringHelper::GetCurrentProfile()
{
	char *profileName = obs_frontend_get_current_profile();
	std::string ret = profileName;
	bfree(profileName);
	return ret;
}

std::string Utils::Obs::StringHelper::GetSourceTypeString(obs_source_t *source)
{
	obs_source_type sourceType = obs_source_get_type(source);

	switch (sourceType) {
		default:
		CASE(OBS_SOURCE_TYPE_INPUT)
		CASE(OBS_SOURCE_TYPE_FILTER)
		CASE(OBS_SOURCE_TYPE_TRANSITION)
		CASE(OBS_SOURCE_TYPE_SCENE)
	}
}

std::string Utils::Obs::StringHelper::GetInputMonitorTypeString(obs_source_t *input)
{
	obs_monitoring_type monitorType = obs_source_get_monitoring_type(input);

	switch (monitorType) {
		default:
		CASE(OBS_MONITORING_TYPE_NONE)
		CASE(OBS_MONITORING_TYPE_MONITOR_ONLY)
		CASE(OBS_MONITORING_TYPE_MONITOR_AND_OUTPUT)
	}
}

std::string Utils::Obs::StringHelper::GetMediaInputStateString(obs_source_t *input)
{
	obs_media_state mediaState = obs_source_media_get_state(input);

	switch (mediaState) {
		default:
		CASE(OBS_MEDIA_STATE_NONE)
		CASE(OBS_MEDIA_STATE_PLAYING)
		CASE(OBS_MEDIA_STATE_OPENING)
		CASE(OBS_MEDIA_STATE_BUFFERING)
		CASE(OBS_MEDIA_STATE_PAUSED)
		CASE(OBS_MEDIA_STATE_STOPPED)
		CASE(OBS_MEDIA_STATE_ENDED)
		CASE(OBS_MEDIA_STATE_ERROR)
	}
}

std::string Utils::Obs::StringHelper::GetLastReplayBufferFilePath()
{
	obs_output_t *output = obs_frontend_get_replay_buffer_output();
	calldata_t cd = {0};
	proc_handler_t *ph = obs_output_get_proc_handler(output);
	proc_handler_call(ph, "get_last_replay", &cd);
	auto ret = calldata_string(&cd, "path");
	calldata_free(&cd);
	obs_output_release(output);
	return ret;
}

std::vector<std::string> Utils::Obs::ListHelper::GetSceneCollectionList()
{
	char** sceneCollections = obs_frontend_get_scene_collections();
	auto ret = ConvertStringArray(sceneCollections);
	bfree(sceneCollections);
	return ret;
}

std::vector<std::string> Utils::Obs::ListHelper::GetProfileList()
{
	char** profiles = obs_frontend_get_profiles();
	auto ret = ConvertStringArray(profiles);
	bfree(profiles);
	return ret;
}

std::vector<obs_hotkey_t *> Utils::Obs::ListHelper::GetHotkeyList()
{
	std::vector<obs_hotkey_t *> ret;

	obs_enum_hotkeys([](void* data, obs_hotkey_id id, obs_hotkey_t* hotkey) {
		auto ret = reinterpret_cast<std::vector<obs_hotkey_t *> *>(data);

		ret->push_back(hotkey);

		return true;
	}, &ret);

	return ret;
}

std::vector<std::string> Utils::Obs::ListHelper::GetHotkeyNameList()
{
	auto hotkeys = GetHotkeyList();

	std::vector<std::string> ret;
	for (auto hotkey : hotkeys) {
		ret.push_back(obs_hotkey_get_name(hotkey));
	}

	return ret;
}

std::vector<json> Utils::Obs::ListHelper::GetSceneList()
{
	obs_frontend_source_list sceneList = {};
	obs_frontend_get_scenes(&sceneList);

	std::vector<json> ret;
	for (size_t i = 0; i < sceneList.sources.num; i++) {
		obs_source_t *scene = sceneList.sources.array[i];
		json sceneJson;
		sceneJson["sceneName"] = obs_source_get_name(scene);
		sceneJson["sceneIndex"] = sceneList.sources.num - (i + 1);
		sceneJson["isGroup"] = obs_source_is_group(scene);
		ret.push_back(sceneJson);
	}

	obs_frontend_source_list_free(&sceneList);

	return ret;
}

std::vector<json> Utils::Obs::ListHelper::GetSceneItemList(obs_scene_t *scene, bool basic)
{
	std::vector<json> ret;

	if (basic) {
		obs_scene_enum_items(scene, [](obs_scene_t* scene, obs_sceneitem_t* sceneItem, void* param) {
			auto ret = reinterpret_cast<std::vector<json>*>(param);

			json item;
			item["sceneItemId"] = obs_sceneitem_get_id(sceneItem);
			// Should be slightly faster than calling obs_sceneitem_get_order_position()
			item["sceneItemIndex"] = ret->size();

			ret->push_back(item);

			return true;
		}, &ret);
	} else {
		obs_scene_enum_items(scene, [](obs_scene_t* scene, obs_sceneitem_t* sceneItem, void* param) {
			auto ret = reinterpret_cast<std::vector<json>*>(param);

			OBSSource itemSource = obs_sceneitem_get_source(sceneItem);

			json item;
			item["sceneItemId"] = obs_sceneitem_get_id(sceneItem);
			item["sceneItemIndex"] = ret->size();
			item["sourceName"] = obs_source_get_name(itemSource);
			item["sourceType"] = StringHelper::GetSourceTypeString(itemSource);
			if (obs_source_get_type(itemSource) == OBS_SOURCE_TYPE_INPUT)
				item["inputKind"] = obs_source_get_id(itemSource);

			ret->push_back(item);

			return true;
		}, &ret);
	}

	return ret;
}

std::vector<json> Utils::Obs::ListHelper::GetTransitionList()
{
	obs_frontend_source_list transitionList = {};
	obs_frontend_get_transitions(&transitionList);

	std::vector<json> ret;
	for (size_t i = 0; i < transitionList.sources.num; i++) {
		obs_source_t *transition = transitionList.sources.array[i];
		json transitionJson;
		transitionJson["transitionName"] = obs_source_get_name(transition);
		transitionJson["transitionKind"] = obs_source_get_id(transition);
		transitionJson["transitionFixed"] = obs_transition_fixed(transition);
		ret.push_back(transitionJson);
	}

	obs_frontend_source_list_free(&transitionList);

	return ret;
}

struct EnumInputInfo {
	std::string inputKind;
	std::vector<json> inputs;
};

std::vector<json> Utils::Obs::ListHelper::GetInputList(std::string inputKind)
{
	EnumInputInfo inputInfo;
	inputInfo.inputKind = inputKind;

	auto inputEnumProc = [](void *param, obs_source_t *input) {
		// Sanity check in case the API changes
		if (obs_source_get_type(input) != OBS_SOURCE_TYPE_INPUT)
			return true;

		auto inputInfo = reinterpret_cast<EnumInputInfo*>(param);

		std::string inputKind = obs_source_get_id(input);

		if (!inputInfo->inputKind.empty() && inputInfo->inputKind != inputKind)
			return true;

		json inputJson;
		inputJson["inputName"] = obs_source_get_name(input);
		inputJson["inputKind"] = inputKind;
		inputJson["unversionedInputKind"] = obs_source_get_unversioned_id(input);

		inputInfo->inputs.push_back(inputJson);
		return true;
	};
	// Actually enumerates only public inputs, despite the name
	obs_enum_sources(inputEnumProc, &inputInfo);

	return inputInfo.inputs;
}

std::vector<std::string> Utils::Obs::ListHelper::GetInputKindList(bool unversioned)
{
	std::vector<std::string> ret;

	size_t idx = 0;
	const char *kind;
	const char *unversioned_kind;
	while (obs_enum_input_types2(idx++, &kind, &unversioned_kind)) {
		uint32_t caps = obs_get_source_output_flags(kind);

		if ((caps & OBS_SOURCE_CAP_DISABLED) != 0)
			continue;

		if (unversioned)
			ret.push_back(unversioned_kind);
		else
			ret.push_back(kind);
	}

	return ret;
}

obs_hotkey_t *Utils::Obs::SearchHelper::GetHotkeyByName(std::string name)
{
	auto hotkeys = ListHelper::GetHotkeyList();

	for (auto hotkey : hotkeys) {
		if (obs_hotkey_get_name(hotkey) == name)
			return hotkey;
	}

	return nullptr;
}
