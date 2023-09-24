import ZeekControl.plugin

class AF_Packet(ZeekControl.plugin.Plugin):
	def __init__(self):
		super(AF_Packet, self).__init__(apiversion=1)

	def name(self):
		return "af_packet"

	def pluginVersion(self):
		return 1

	def init(self):
		# Only use the plugin if there is a worker using AF_PACKET for load balancing.
		for nn in self.nodes():
			if nn.type == "worker" and nn.interface.startswith("af_packet::") and nn.lb_procs:
				return True

		return False

	def nodeKeys(self):
		return [
			"buffer_size",
			"block_size",
			"block_timeout",
			"enable_hw_timestamping",
			"enable_fanout",
			"enable_defrag",
			"fanout_mode",
			"fanout_id",
			"link_type",
			"checksum_validation_mode",
			]

	def zeekctl_config(self):
		script = ""

		# Add custom configuration values per worker.
		for nn in self.nodes():
			if nn.type != "worker" or not nn.lb_procs:
				continue

			params = ""

			for p, v in vars(nn).items():
				if p.startswith('af_packet_') and v:
					params += f"\n  redef AF_Packet::{p[10:]} = {v};"

			if params:
				script += "\n@if( peer_description == \"%s\" ) %s\n@endif" % (nn.name, params)

		return script
