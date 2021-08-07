#include "D3D11Framework.h"
#include "Command.h"

Command::Command()
{
	constant_buffers.reserve(D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
	shader_resources.reserve(D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT);
	sampler_states.reserve(D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT);
	render_targets.reserve(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	Clear();
}

void Command::Clear()
{
	command_type = CommandType::Unknown;
	pass_name = NOT_ASSIGNED_STR;

	vertex_buffer = nullptr;
	vertex_stride = 0;
	vertex_offset = 0;
	vertex_count = 0;

	index_buffer = nullptr;
	index_offset = 0;
	index_count = 0;

	input_layout = nullptr;
	primitive_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vertex_shader = nullptr;
	pixel_shader = nullptr;

	constant_buffers.clear();
	constant_buffer_shader_scope = ShaderScope::Unknown;
	constant_buffer_slot = 0;
	constant_buffer_count = 0;

	shader_resources.clear();
	shader_resource_shader_scope = ShaderScope::Unknown;
	shader_resource_slot = 0;
	shader_resource_count = 0;

	sampler_states.clear();
	sampler_state_shader_scope = ShaderScope::Unknown;
	sampler_state_slot = 0;
	sampler_state_count = 0;

	rasterizer_state = nullptr;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	blend_state = nullptr;
	depth_stencil_state = nullptr;

	render_targets.clear();
	render_target_count = 0;

	depth_stencil_target = nullptr;

	clear_render_target = nullptr;
	clear_color = Color4::Zero;

	clear_depth_stencil_target = nullptr;
	clear_flags = 0;
	clear_stencil = 0;
	clear_depth = 0.0f;
}
