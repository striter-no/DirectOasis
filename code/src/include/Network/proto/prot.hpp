#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utils/string.hpp>

enum class StatusCode {
    Success = 0,
    NotAuthorized = 1,
    InternalServerError = 2,
    EndpointPathIsNotFound = 3,
    Forbiden = 4,
    ActionIsNotDoableInThisContext = 5,
    InvalidMessageFormat = 6,
    InvalidValue = 7,
    InvalidDataType = 8,
    AddresseeDoesNotExist = 9,
    ServerIsEmpty = 10,
    UnknownStatusCode = -1
};

class Message {
public:
    std::string endpoint;
    std::string datatype;
    std::string msgspec;
    std::string to_cli_pub_uid;
    std::string uid;
    std::string content;
    std::string statuscode;
    std::string cli_from;
    std::string pub_uid;

    Message() : endpoint(""), datatype(""), msgspec(""), to_cli_pub_uid(""),
                uid(""), content(""), statuscode(""), cli_from(""), pub_uid("") {}

    std::string toString() const {
        // Implement string representation if needed
        return "";
    }
};

class ProtMessage {
public:
    static std::string from_user(
        const std::string& endpoint,
        const std::string& datatype,
        const std::string& msgspec,
        const std::string& to_cli_pub_uid,
        const std::string& uid,
        const std::string& pub_uid,
        const std::string& content
    ) {
        return "EndPoint:" + endpoint + "\n" +
               "Data-Type:" + datatype + "\n" +
               "Msg-Spec:" + msgspec + "\n" +
               "To-Cli-UID:" + to_cli_pub_uid + "\n" +
               "UID:" + uid + "\n" +
               "PUB-UID:" + pub_uid + "\n" +
               "Content:\n" + content + "\n";
    }

    static std::string from_server(
        const std::string& statuscode,
        const std::string& datatype = "text",
        const std::string& msgspec = "server",
        const std::string& cli_from = "-1",
        const std::string& content = ""
    ) {
        return "StatusCode:" + statuscode + "\n" +
               "Data-Type:" + datatype + "\n" +
               "Msg-Spec:" + msgspec + "\n" +
               "Cli-From:" + cli_from + "\n" +
               "Content:\n" + content;
    }

    static Message decode(const std::string& data) {
        Message out;
        std::string glob_content;
        bool content_flag = false;

        std::istringstream stream(data);
        std::string line;
        while (std::getline(stream, line)) {
            auto pos = line.find(':');
            std::string header = line.substr(0, pos);
            std::string content = (pos != std::string::npos) ? line.substr(pos + 1) : "";

            if (header == "Content") {
                content_flag = true;
                continue;
            }

            if (content_flag) {
                glob_content += line + "\n";
            } else {
                if (header == "EndPoint") out.endpoint = content;
                else if (header == "Data-Type") out.datatype = content;
                else if (header == "Msg-Spec") out.msgspec = content;
                else if (header == "To-Cli-UID") out.to_cli_pub_uid = content;
                else if (header == "UID") out.uid = content;
                else if (header == "StatusCode") out.statuscode = content;
                else if (header == "Cli-From") out.cli_from = content;
                else if (header == "PUB-UID") out.pub_uid = content;
            }
        }

        out.content = utils::str::strip(glob_content, '\n');
        return out;
    }

    static int int_status(StatusCode status) {
        return static_cast<int>(status);
    }

    static std::string string_status(StatusCode status) {
        switch (status) {
            case StatusCode::Success: return "Success";
            case StatusCode::NotAuthorized: return "Not authorized";
            case StatusCode::InternalServerError: return "Internal Server Error";
            case StatusCode::EndpointPathIsNotFound: return "Endpoint path is not found";
            case StatusCode::Forbiden: return "Forbiden";
            case StatusCode::ActionIsNotDoableInThisContext: return "The action is not doable in this context";
            case StatusCode::InvalidMessageFormat: return "Invalid message format";
            case StatusCode::InvalidValue: return "Invalid value";
            case StatusCode::InvalidDataType: return "Invalid data type";
            case StatusCode::AddresseeDoesNotExist: return "Addressee does not exist";
            case StatusCode::ServerIsEmpty: return "Server is empty";
            default: return "Unknown status code";
        }
    }

    static StatusCode status(const std::string& status) {
        int stat = std::stoi(status);
        switch (stat) {
            case 0: return StatusCode::Success;
            case 1: return StatusCode::NotAuthorized;
            case 2: return StatusCode::InternalServerError;
            case 3: return StatusCode::EndpointPathIsNotFound;
            case 4: return StatusCode::Forbiden;
            case 5: return StatusCode::ActionIsNotDoableInThisContext;
            case 6: return StatusCode::InvalidMessageFormat;
            case 7: return StatusCode::InvalidValue;
            case 8: return StatusCode::InvalidDataType;
            case 9: return StatusCode::AddresseeDoesNotExist;
            case 10: return StatusCode::ServerIsEmpty;
            default: return StatusCode::UnknownStatusCode;
        }
    }
};