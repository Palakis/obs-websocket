#include "RequestHandler.h"

#include "../plugin-macros.generated.h"

const std::map<std::string, RequestMethodHandler> RequestHandler::_handlerMap
{
	// General
	{"GetVersion", &RequestHandler::GetVersion},
	{"BroadcastCustomEvent", &RequestHandler::BroadcastCustomEvent},
	{"GetHotkeyList", &RequestHandler::GetHotkeyList},
	{"TriggerHotkeyByName", &RequestHandler::TriggerHotkeyByName},
	{"TriggerHotkeyByKeySequence", &RequestHandler::TriggerHotkeyByKeySequence},
	{"GetStudioModeEnabled", &RequestHandler::GetStudioModeEnabled},
	{"SetStudioModeEnabled", &RequestHandler::SetStudioModeEnabled},
	{"Sleep", &RequestHandler::Sleep},

	// Config
	{"GetSceneCollectionList", &RequestHandler::GetSceneCollectionList},
	{"SetCurrentSceneCollection", &RequestHandler::SetCurrentSceneCollection},
	{"GetProfileList", &RequestHandler::GetProfileList},
	{"SetCurrentProfile", &RequestHandler::SetCurrentProfile},
	{"GetProfileParameter", &RequestHandler::GetProfileParameter},
	{"SetProfileParameter", &RequestHandler::SetProfileParameter},

	// Sources
	{"GetSourceActive", &RequestHandler::GetSourceActive},
};

RequestResult RequestHandler::ProcessRequest(const Request& request)
{
	if (!request.RequestData.is_null() && !request.RequestData.is_object())
		return RequestResult::Error(RequestStatus::InvalidRequestParameterDataType, "Your request data is not an object.");

	if (request.RequestType.empty())
		return RequestResult::Error(RequestStatus::MissingRequestType, "Your request is missing a `requestType`");

	RequestMethodHandler handler;
	try {
		handler = _handlerMap.at(request.RequestType);
	} catch (const std::out_of_range& oor) {
		return RequestResult::Error(RequestStatus::UnknownRequestType, "Your request type is not valid.");
	}

	return std::bind(handler, this, std::placeholders::_1)(request);
}

std::vector<std::string> RequestHandler::GetRequestList()
{
	std::vector<std::string> ret;
	for (auto const& [key, val] : _handlerMap) {
		ret.push_back(key);
	}

	return ret;
}
