from enum import Enum

class Message:
    def __init__(
        self,
        endpoint = "",
        datatype = "",
        msgspec = "",
        to_cli_pub_uid = "",
        uid = "",
        content = "",
        statuscode = "",
        cli_from = "",
        pub_uid = "",
    ):
        self.endpoint = endpoint
        self.datatype = datatype
        self.msgspec = msgspec
        self.to_cli_pub_uid = to_cli_pub_uid
        self.uid = uid
        self.content = content
        self.statuscode = statuscode
        self.cli_from = cli_from
        self.pub_uid = pub_uid
    
    def __str__(self):
        pass

class StatusCode(Enum):
    Success = 0
    NotAuthorized = 1
    InternalServerError = 2
    EndpointPathIsNotFound = 3
    Forbiden = 4
    ActionIsNotDoableInThisContext = 5
    InvalidMessageFormat = 6
    InvalidValue = 7
    InvalidDataType = 8
    AddresseeDoesNotExist = 9
    ServerIsEmpty = 10
    UnknownStatusCode = -1

class ProtMessage:
    @staticmethod
    def from_user(
        endpoint: str,
        datatype: str,
        msgspec:  str,
        to_cli_pub_uid: str,
        uid: str,
        pub_uid: str,
        content: str
    ):
        return f"""EndPoint:{endpoint}
Data-Type:{datatype}
Msg-Spec:{msgspec}
To-Cli-UID:{to_cli_pub_uid}
UID:{uid}
PUB-UID:{pub_uid}
Content: 
{content}
"""
    @staticmethod
    def from_server(
        statuscode: str,
        datatype: str = "text",
        msgspec: str = "server",
        cli_from: str = "-1",
        content: str = ""
    ):
        return f"""StatusCode:{statuscode}
Data-Type:{datatype}
Msg-Spec:{msgspec}
Cli-From:{cli_from}
Content:
{content}
"""
    
    @staticmethod
    def decode(
        data: str
    ) -> Message:
        out = Message()
        content_flag = False
        glob_content = ""
        for line in data.splitlines():
            header = line.split(':')[0]
            content = ""

            if header == "Content":
                content_flag = True
                continue

            elif content_flag:
                glob_content += line + "\n"
            else:
                content = ":".join(line.split(':')[1:])
                
                match header:
                    case "EndPoint":   out.endpoint   = content
                    case "Data-Type":  out.datatype   = content
                    case "Msg-Spec":   out.msgspec    = content
                    case "To-Cli-UID": out.to_cli_pub_uid = content
                    case "UID":        out.uid        = content
                    case "StatusCode": out.statuscode = content
                    case "Cli-From":   out.cli_from   = content
                    case "PUB-UID":    out.pub_uid    = content
        
        out.content = glob_content.strip()
        return out
    
    @staticmethod
    def int_status(
        status: StatusCode
    ):
        return status.value

    @staticmethod
    def string_status(
        status: StatusCode
    ):
        match status:
            case StatusCode.Success:  return "Success"
            case StatusCode.NotAuthorized:  return "Not authorized"
            case StatusCode.InternalServerError:  return "Internal Server Error"
            case StatusCode.EndpointPathIsNotFound:  return "Endpoint path is not found"
            case StatusCode.Forbiden:  return "Forbiden"
            case StatusCode.ActionIsNotDoableInThisContext:  return "The action is not doable in this context"
            case StatusCode.InvalidMessageFormat:  return "Invalid message format"
            case StatusCode.InvalidValue:  return "Invalid value"
            case StatusCode.InvalidDataType:  return "Invalid data type"
            case StatusCode.AddresseeDoesNotExist:  return "Addressee does not exist"
            case StatusCode.ServerIsEmpty: return "Server is empty"
            case _:  return "Unknown status code"
    
    @staticmethod
    def status(
        status: str
    ):
        status = int(status)
        match status:
            case 0: return StatusCode.Success
            case 1: return StatusCode.NotAuthorized
            case 2: return StatusCode.InternalServerError
            case 3: return StatusCode.EndpointPathIsNotFound
            case 4: return StatusCode.Forbiden
            case 5: return StatusCode.ActionIsNotDoableInThisContext
            case 6: return StatusCode.InvalidMessageFormat
            case 7: return StatusCode.InvalidValue
            case 8: return StatusCode.InvalidDataType
            case 9: return StatusCode.AddresseeDoesNotExist
            case 10: return StatusCode.ServerIsEmpty
            case _:  return StatusCode.UnknownStatusCode