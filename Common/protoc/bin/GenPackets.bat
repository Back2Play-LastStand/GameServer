pushd %~dp0

protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

protoc.exe -I=./ --csharp_out=./ ./Enum.proto
protoc.exe -I=./ --csharp_out=./ ./Struct.proto
protoc.exe -I=./ --csharp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=REQ_ --send=RES_
GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=RES_ --send=REQ_
IF ERRORLEVEL 1 PAUSE

XCOPY /Y Enum.pb.h "../../../MainServer/Protocol"
XCOPY /Y Enum.pb.cc "../../../MainServer/Protocol"
XCOPY /Y Struct.pb.h "../../../MainServer/Protocol"
XCOPY /Y Struct.pb.cc "../../../MainServer/Protocol"
XCOPY /Y Protocol.pb.h "../../../MainServer/Protocol"
XCOPY /Y Protocol.pb.cc "../../../MainServer/Protocol"
XCOPY /Y ServerPacketHandler.h "../../../MainServer"

XCOPY /Y Enum.cs "C:\Users\User\Documents\GitHub\Topdown-Shooting\Assets\Scripts\Packet\Protocol"
XCOPY /Y Struct.cs "C:\Users\User\Documents\GitHub\Topdown-Shooting\Assets\Scripts\Packet\Protocol"
XCOPY /Y Protocol.cs "C:\Users\User\Documents\GitHub\Topdown-Shooting\Assets\Scripts\Packet\Protocol"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h
DEL /Q /F *.cs

PAUSE