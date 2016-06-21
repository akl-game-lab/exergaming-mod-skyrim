// webserviceTest.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Windows.h"
#include <iostream>

#include <iomanip>
#include <sstream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

std::string NORMAL_FETCH = "0";
std::string FORCE_FETCH = "1";

std::string URL_BASE = "http://ec2-54-252-163-152.ap-southeast-2.compute.amazonaws.com:3000/users/";

/*======================
	Helper functions
======================*/

// Convert a wide Unicode string to an UTF8 string
std::string toString(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

/*======================
	Main functions
======================*/

/*
int main(int argc, char* argv[])
{
	std::string gameID = argv[0];
	std::string userName = argv[1];
	std::string fromDate = argv[2];
	std::string toDate = argv[3];

	std::string url = "http://exergaming.isaac.techrus.co.nz/poc_api/get-workout/" + gameID + "/" + userName + "/" + fromDate + "/" + toDate;
	uri fullUri(conversions::to_string_t(url));

	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("Raw_Data.xml")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		// Create http_client to send the request.
		http_client client(fullUri);

		// Build request URI and start the request.
		uri_builder builder(U("/search"));
		builder.append_query(U("q"), U("Casablanca CodePlex"));
		return client.request(methods::GET, builder.to_string());
	})

		// Handle response headers arriving.
		.then([=](http_response response)
	{
		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
	});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
	}
	return 0;
}*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	LPWSTR *szArgList;
	int argCount;

	std::ofstream reportFile("Service_Report.txt");

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList == NULL)
	{
		return 10;
	}

	std::string type = toString(szArgList[1]);
	std::string username = toString(szArgList[2]);
	std::string url = URL_BASE + username + "/forceUpdate";

	if (type == "NORMAL"){
		std::string fromDate = toString(szArgList[3]);
		std::string toDate = toString(szArgList[4]);
		url = URL_BASE + username + "/workouts/" + fromDate + "/" + toDate;
	}

	LocalFree(szArgList);

	uri fullUri(conversions::to_string_t(url));

	reportFile << "url:" + url + "\n";

	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("Raw_Data.xml")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		// Create http_client to send the request.
		http_client client(fullUri);

		// Start the request.
		return client.request(methods::GET);
	})

		// Handle response headers arriving.
		.then([=](http_response response)
	{
		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
	});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
	}
	reportFile << "finished";
	reportFile.close();
	return 0;
}