// Copyright (c) 2024 Thirdweb. All Rights Reserved.

#pragma once

#include "HttpModule.h"
#include "TappyFowlEPPUSettings.h"

namespace TappyFowl
{
	namespace JSON
	{
		static TSharedPtr<FJsonObject> StringToJsonObject(const FString& JsonString)
		{
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			// ReSharper disable once CppTooWideScopeInitStatement
			const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				return JsonObject;
			}
			return TSharedPtr<FJsonObject>();
		}

		static FString JsonObjectToString(const TSharedPtr<FJsonObject>& JsonObject)
		{
			FString Out;
			const TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Out);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
			return Out;
		}
	}

	namespace HTTP
	{
		static TSharedRef<class IHttpRequest> CreateAPIRequest(const FString& Verb, const FString& Endpoint, const TMap<FString, FString>& Headers = {}, const TSharedPtr<FJsonObject>& Content = nullptr)
		{
			const UTappyFowlEPPUSettings* Settings = UTappyFowlEPPUSettings::Get();
			const TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
			Request->SetVerb(Verb.ToUpper());
			Request->SetURL(Settings->GetApiChainUrl() + Endpoint);

			Request->SetHeader(TEXT("Accept"), TEXT("application/vnd.com.thirdweb.tappyfowl.api.v1+json"));
			Request->SetHeader("Authorization", Settings->GetApiKey());
			if (Verb.ToUpper().Equals(TEXT("POST")))
			{
				Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
				Request->SetContentAsString(JSON::JsonObjectToString(Content));
			}
			for (const TTuple<FString, FString>& Header : Headers)
			{
				Request->SetHeader(Header.Key, Header.Value);
			}
			return Request;
		}

		static TSharedRef<IHttpRequest> Get(const FString& Endpoint) { return CreateAPIRequest(TEXT("GET"), Endpoint); }

		static TSharedRef<IHttpRequest> Post(const FString& Endpoint, const TSharedPtr<FJsonObject>& Content = nullptr, const TMap<FString, FString>& Headers = {})
		{
			return CreateAPIRequest(TEXT("POST"), Endpoint, Headers, Content);
		}
	}
}
