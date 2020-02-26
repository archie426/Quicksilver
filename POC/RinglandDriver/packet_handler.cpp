#include "packet_handler.h"
#include "imports.h"
#include "log.h"
#include "globals.h"

static uint64_t handle_copy_memory(const PacketCopyMemory& packet)
{
	PEPROCESS dest_process = nullptr;
	PEPROCESS src_process = nullptr;

	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.dest_process_id), &dest_process)))
	{
		return uint64_t(STATUS_INVALID_CID);
	}

	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.src_process_id), &src_process)))
	{
		ObDereferenceObject(dest_process);
		return uint64_t(STATUS_INVALID_CID);
	}

	SIZE_T   return_size = 0;
	NTSTATUS status = MmCopyVirtualMemory(
		src_process,
		(void*)packet.src_address,
		dest_process,
		(void*)packet.dest_address,
		packet.size,
		UserMode,
		&return_size
	);

	ObDereferenceObject(dest_process);
	ObDereferenceObject(src_process);

	return uint64_t(status);
}

static uint64_t handle_get_base_address(const PacketGetBaseAddress& packet)
{
	PEPROCESS process = nullptr;
	NTSTATUS  status = PsLookupProcessByProcessId(HANDLE(packet.process_id), &process);

	if (!NT_SUCCESS(status))
		return 0;

	const auto base_address = uint64_t(PsGetProcessSectionBaseAddress(process));
	ObDereferenceObject(process);

	return base_address;
}

static uint64_t handle_echo(const PacketEcho& packet) {
	log(packet.text);

	return 0xB000B135;
}

static uint64_t handle_close_server(const PacketCloseServer& packet) {
	log("Recieved packet to close server, shutting down.");

	if (packet.magic == close_server_magic) {
		shut_down_server = true;
	}

	return 0xB16B00B5;
}

uint64_t handle_incoming_packet(const Packet& packet)
{
	switch (packet.header.type)
	{
	case PacketType::packet_copy_memory:
		return handle_copy_memory(packet.data.copy_memory);

	case PacketType::packet_get_base_address:
		return handle_get_base_address(packet.data.get_base_address);

	case PacketType::packet_echo:
		return handle_echo(packet.data.echo);

	case PacketType::packet_close_server:
		return handle_close_server(packet.data.close_server);

	default:
		break;
	}

	return uint64_t(STATUS_NOT_IMPLEMENTED);
}

// Send completion packet.
bool complete_request(const SOCKET client_connection, const uint64_t result)
{
	Packet packet{ };

	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_completed;
	packet.data.completed.result = result;

	return send(client_connection, &packet, sizeof(packet), 0) != SOCKET_ERROR;
}