// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

extern "C" {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <errno.h>          // errorno
#include <unistd.h>         // close()

#include <net/ethernet.h>      // ETH_P_ALL
#include <linux/if.h>          // ifreq
#include <linux/if_packet.h>   // AF_PACKET, etc.
#include <linux/sockios.h>     // SIOCSHWTSTAMP
#include <linux/net_tstamp.h>  // hwtstamp_config
}
#define bpf_insn pcap_bpf_insn
#include "zeek/iosource/PktSrc.h"
#undef bpf_insn

#include <bpf/libbpf.h>
#include "RX_Ring.h"

namespace zeek::iosource::pktsrc {

class AF_PacketSource : public zeek::iosource::PktSrc {
public:
	/**
	 * Constructor.
	 *
	 * path: Name of the interface to open (the AF_Packet source doesn't
	 * support reading from files).
	 *
	 * is_live: Must be true (the AF_Packet source doesn't support offline
	 * operation).
	 */
	AF_PacketSource(const std::string& path, bool is_live);

	/**
	 * Destructor.
	 */
	~AF_PacketSource() override;

	static PktSrc* InstantiateAF_Packet(const std::string& path, bool is_live);

protected:
	// PktSrc interface.
	void Open() override;
	void Close() override;
	bool ExtractNextPacket(zeek::Packet* pkt) override;
	void DoneWithPacket() override;
	bool PrecompileFilter(int index, const std::string& filter) override;
	bool SetFilter(int index) override;
	void Statistics(Stats* stats) override;

private:
	Properties props;
	Stats stats;

	int current_filter;
	unsigned int num_discarded;
	int checksum_mode;

	int socket_fd;
	RX_Ring *rx_ring;
	struct pcap_pkthdr current_hdr;

	struct InterfaceInfo {
		int index = -1;
		int flags = 0;

		bool Valid () { return index >= 0; }
		bool IsUp() { return flags & IFF_UP; }
	};

	InterfaceInfo GetInterfaceInfo(const std::string& path);
	bool BindInterface(const InterfaceInfo& info);
	bool EnablePromiscMode(const InterfaceInfo& info);
	bool ConfigureFanoutGroup(bool enabled, bool defrag);
	bool ConfigureFanoutLoadBalancer(const char* ebpf_lb_file);
	bool LoadEBPFLoadBalanceFile(const char* ebpf_lb_file, int& fd);
	struct bpf_program *FindProgramInSection(struct bpf_object* bpfobj, const char* section);
	bool ConfigureHWTimestamping(bool enabled);
	uint32_t GetFanoutMode(bool defrag);
};

}
