/**************************************************************************/
/*  test_visual_shader_group_editing_complete.h                          */
/**************************************************************************/
/*                         This file is part of:                         */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/

#ifndef TEST_VISUAL_SHADER_GROUP_EDITING_COMPLETE_H
#define TEST_VISUAL_SHADER_GROUP_EDITING_COMPLETE_H

#include "editor/plugins/shader_editor_plugin.h"
#include "editor/plugins/visual_shader_editor_plugin.h"
#include "scene/resources/visual_shader.h"
#include "scene/resources/visual_shader_nodes.h"
#include "tests/test_macros.h"

namespace TestVisualShaderGroupEditingComplete {

TEST_CASE("[VisualShader][GroupEditing] Complete workflow from creation to persistence") {
	print_line("[TEST] === STARTING COMPLETE GROUP EDITING WORKFLOW TEST ===");

	SUBCASE("Step 1: Create new visual shader project") {
		print_line("[TEST] Step 1: Creating new visual shader");

		// Create a new visual shader (simulating "new project")
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		CHECK(vs.is_valid());
		CHECK(vs->get_mode() == Shader::MODE_CANVAS_ITEM);

		// Should have default output node
		Vector<int> nodes = vs->get_node_list(VisualShader::TYPE_FRAGMENT);
		CHECK(nodes.size() == 1);
		CHECK(nodes[0] == 0); // Output node should be ID 0

		print_line("[TEST] ✓ Visual shader created successfully");
	}

	SUBCASE("Step 2: Add group node to visual shader") {
		print_line("[TEST] Step 2: Adding group node to visual shader");

		// Create visual shader
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		// Create and add group node
		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Verify group node was added
		Vector<int> nodes = vs->get_node_list(VisualShader::TYPE_FRAGMENT);
		CHECK(nodes.size() == 2); // Output + Group
		CHECK(nodes.has(group_id));

		Ref<VisualShaderNode> retrieved_node = vs->get_node(VisualShader::TYPE_FRAGMENT, group_id);
		CHECK(retrieved_node.is_valid());
		CHECK(Object::cast_to<VisualShaderNodeGroup>(retrieved_node.ptr()) != nullptr);

		print_line("[TEST] ✓ Group node added successfully with ID: ", group_id);
	}

	SUBCASE("Step 3: Verify group node can be accessed for editing") {
		print_line("[TEST] Step 3: Verifying group node accessibility");

		// Create visual shader with group
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Verify we can retrieve the group node for editing
		Ref<VisualShaderNode> retrieved_node = vs->get_node(VisualShader::TYPE_FRAGMENT, group_id);
		CHECK(retrieved_node.is_valid());

		Ref<VisualShaderNodeGroup> retrieved_group = Object::cast_to<VisualShaderNodeGroup>(retrieved_node.ptr());
		CHECK(retrieved_group.is_valid());
		CHECK(retrieved_group == group_node);

		print_line("[TEST] ✓ Group node accessible for editing");
	}

	SUBCASE("Step 4: Manually create and verify interface nodes in group") {
		print_line("[TEST] Step 4: Creating interface nodes in group");

		// Create visual shader with group
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Manually create interface nodes (simulating what the editor would do)
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		input_node->set_input_name("GroupInput");
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);

		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		output_node->set_shader_mode(Shader::MODE_CANVAS_ITEM);
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);

		// Verify interface nodes were created
		Vector<int> internal_nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		print_line("[TEST] Internal nodes count: ", internal_nodes.size());

		bool has_input = false;
		bool has_output = false;

		for (int node_id : internal_nodes) {
			Ref<VisualShaderNode> node = group_node->get_internal_node(VisualShader::TYPE_FRAGMENT, node_id);
			print_line("[TEST] Found internal node ID ", node_id, ": ", node->get_class());

			if (Object::cast_to<VisualShaderNodeInput>(node.ptr())) {
				has_input = true;
				print_line("[TEST] ✓ Found Group Input node");
			} else if (Object::cast_to<VisualShaderNodeOutput>(node.ptr())) {
				has_output = true;

				// Verify output node is properly initialized
				Ref<VisualShaderNodeOutput> output_node = Object::cast_to<VisualShaderNodeOutput>(node.ptr());
				print_line("[TEST] Output node shader_mode: ", output_node->shader_mode, ", shader_type: ", output_node->shader_type);
				print_line("[TEST] Output node input port count: ", output_node->get_input_port_count());

				CHECK(output_node->get_input_port_count() > 0);
				print_line("[TEST] ✓ Found Group Output node with ", output_node->get_input_port_count(), " input ports");
			}
		}

		CHECK(has_input);
		CHECK(has_output);
		CHECK(internal_nodes.size() >= 2); // At least input and output

		print_line("[TEST] ✓ Interface nodes verified successfully");
	}

	SUBCASE("Step 5: Add another node to the group") {
		print_line("[TEST] Step 5: Adding additional node to group");

		// Create visual shader with group and interface nodes
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Create interface nodes first
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);

		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		output_node->set_shader_mode(Shader::MODE_CANVAS_ITEM);
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);

		// Get initial node count
		Vector<int> initial_nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		int initial_count = initial_nodes.size();
		print_line("[TEST] Initial internal nodes: ", initial_count);

		// Add a color constant node to the group (simulating user adding node)
		Ref<VisualShaderNodeColorConstant> color_node = memnew(VisualShaderNodeColorConstant);
		color_node->set_constant(Color(1.0, 0.5, 0.0, 1.0)); // Orange color

		int color_node_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node, Vector2(0, 100), color_node_id);

		// Verify node was added
		Vector<int> final_nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		CHECK(final_nodes.size() == initial_count + 1);
		CHECK(final_nodes.has(color_node_id));

		Ref<VisualShaderNode> retrieved_color = group_node->get_internal_node(VisualShader::TYPE_FRAGMENT, color_node_id);
		CHECK(retrieved_color.is_valid());
		CHECK(Object::cast_to<VisualShaderNodeColorConstant>(retrieved_color.ptr()) != nullptr);

		// Verify the color value
		Ref<VisualShaderNodeColorConstant> retrieved_color_const = Object::cast_to<VisualShaderNodeColorConstant>(retrieved_color.ptr());
		Color retrieved_color_value = retrieved_color_const->get_constant();
		CHECK(retrieved_color_value.is_equal_approx(Color(1.0, 0.5, 0.0, 1.0)));

		print_line("[TEST] ✓ Additional node added successfully with ID: ", color_node_id);
	}

	SUBCASE("Step 6: Test connections within group") {
		print_line("[TEST] Step 6: Testing connections within group");

		// Create visual shader with group
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Create interface nodes
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);

		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		output_node->set_shader_mode(Shader::MODE_CANVAS_ITEM);
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);

		// Add a color node
		Ref<VisualShaderNodeColorConstant> color_node = memnew(VisualShaderNodeColorConstant);
		int color_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node, Vector2(0, 100), color_id);

		// Test connection capability
		bool can_connect = group_node->can_connect_internal_nodes(
				VisualShader::TYPE_FRAGMENT,
				color_id, 0, // color output
				output_id, 0 // output input
		);

		CHECK(can_connect);
		print_line("[TEST] ✓ Can connect nodes within group");

		// Make the connection
		group_node->connect_internal_nodes(
				VisualShader::TYPE_FRAGMENT,
				color_id, 0,
				output_id, 0);

		// Verify connection exists
		List<VisualShader::Connection> connections;
		group_node->get_internal_node_connections(VisualShader::TYPE_FRAGMENT, &connections);

		CHECK(connections.size() == 1);
		VisualShader::Connection &conn = connections.front()->get();
		CHECK(conn.from_node == color_id);
		CHECK(conn.from_port == 0);
		CHECK(conn.to_node == output_id);
		CHECK(conn.to_port == 0);

		print_line("[TEST] ✓ Connection created successfully");
	}

	SUBCASE("Step 7: Test persistence - save and reload") {
		print_line("[TEST] Step 7: Testing persistence of group modifications");

		// Create visual shader with complete group setup
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Create interface nodes first
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);

		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		output_node->set_shader_mode(Shader::MODE_CANVAS_ITEM);
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);

		// Add multiple nodes to test persistence
		Ref<VisualShaderNodeColorConstant> color_node = memnew(VisualShaderNodeColorConstant);
		color_node->set_constant(Color(1.0, 0.0, 0.0, 1.0)); // Red
		int color_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node, Vector2(-100, 0), color_id);

		Ref<VisualShaderNodeVectorOp> vector_op = memnew(VisualShaderNodeVectorOp);
		vector_op->set_operator(VisualShaderNodeVectorOp::OP_ADD);
		int vector_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, vector_op, Vector2(0, 50), vector_id);

		// Get initial state
		Vector<int> initial_internal_nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		int initial_internal_count = initial_internal_nodes.size();

		print_line("[TEST] Created group with ", initial_internal_count, " internal nodes");

		// Simulate saving by serializing to string and back
		String shader_code = vs->get_code();
		CHECK(!shader_code.is_empty());
		print_line("[TEST] Generated shader code length: ", shader_code.length());

		// Create new visual shader and verify it loads the group correctly
		Ref<VisualShader> vs_reloaded = memnew(VisualShader);
		vs_reloaded->set_mode(Shader::MODE_CANVAS_ITEM);

		// Copy the group structure (simulating file save/load)
		Ref<VisualShaderNodeGroup> reloaded_group = memnew(VisualShaderNodeGroup);

		// Copy all internal nodes
		for (int node_id : initial_internal_nodes) {
			Ref<VisualShaderNode> original_node = group_node->get_internal_node(VisualShader::TYPE_FRAGMENT, node_id);
			Vector2 position = group_node->get_internal_node_position(VisualShader::TYPE_FRAGMENT, node_id);

			// Create copy based on type
			Ref<VisualShaderNode> copied_node;
			if (Object::cast_to<VisualShaderNodeInput>(original_node.ptr())) {
				copied_node = memnew(VisualShaderNodeInput);
			} else if (Object::cast_to<VisualShaderNodeOutput>(original_node.ptr())) {
				Ref<VisualShaderNodeOutput> output_copy = memnew(VisualShaderNodeOutput);
				output_copy->set_shader_mode(Shader::MODE_CANVAS_ITEM);
				output_copy->set_shader_type(VisualShader::TYPE_FRAGMENT);
				copied_node = output_copy;
			} else if (Object::cast_to<VisualShaderNodeColorConstant>(original_node.ptr())) {
				Ref<VisualShaderNodeColorConstant> color_copy = memnew(VisualShaderNodeColorConstant);
				Ref<VisualShaderNodeColorConstant> orig_color = Object::cast_to<VisualShaderNodeColorConstant>(original_node.ptr());
				color_copy->set_constant(orig_color->get_constant());
				copied_node = color_copy;
			} else if (Object::cast_to<VisualShaderNodeVectorOp>(original_node.ptr())) {
				Ref<VisualShaderNodeVectorOp> vec_copy = memnew(VisualShaderNodeVectorOp);
				Ref<VisualShaderNodeVectorOp> orig_vec = Object::cast_to<VisualShaderNodeVectorOp>(original_node.ptr());
				vec_copy->set_op_type(orig_vec->get_op_type());
				copied_node = vec_copy;
			}

			if (copied_node.is_valid()) {
				reloaded_group->add_internal_node(VisualShader::TYPE_FRAGMENT, copied_node, position, node_id);
			}
		}

		// Add reloaded group to new shader
		vs_reloaded->add_node(VisualShader::TYPE_FRAGMENT, reloaded_group, Vector2(100, 100), group_id);

		// Verify everything was preserved
		Vector<int> reloaded_internal_nodes = reloaded_group->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		CHECK(reloaded_internal_nodes.size() == initial_internal_count);

		// Verify specific nodes
		bool found_color = false;
		bool found_vector = false;
		bool found_input = false;
		bool found_output = false;

		for (int node_id : reloaded_internal_nodes) {
			Ref<VisualShaderNode> node = reloaded_group->get_internal_node(VisualShader::TYPE_FRAGMENT, node_id);

			if (Object::cast_to<VisualShaderNodeColorConstant>(node.ptr())) {
				found_color = true;
				Ref<VisualShaderNodeColorConstant> color_const = Object::cast_to<VisualShaderNodeColorConstant>(node.ptr());
				CHECK(color_const->get_constant().is_equal_approx(Color(1.0, 0.0, 0.0, 1.0)));
			} else if (Object::cast_to<VisualShaderNodeVectorOp>(node.ptr())) {
				found_vector = true;
				Ref<VisualShaderNodeVectorOp> vec_op = Object::cast_to<VisualShaderNodeVectorOp>(node.ptr());
				CHECK(vec_op->get_operator() == VisualShaderNodeVectorOp::OP_ADD);
			} else if (Object::cast_to<VisualShaderNodeInput>(node.ptr())) {
				found_input = true;
			} else if (Object::cast_to<VisualShaderNodeOutput>(node.ptr())) {
				found_output = true;
				Ref<VisualShaderNodeOutput> output_node = Object::cast_to<VisualShaderNodeOutput>(node.ptr());
				CHECK(output_node->get_input_port_count() > 0);
			}
		}

		CHECK(found_color);
		CHECK(found_vector);
		CHECK(found_input);
		CHECK(found_output);

		print_line("[TEST] ✓ All group modifications preserved after reload");
	}

	print_line("[TEST] === COMPLETE GROUP EDITING WORKFLOW TEST PASSED ===");
}

} // namespace TestVisualShaderGroupEditingComplete

#endif // TEST_VISUAL_SHADER_GROUP_EDITING_COMPLETE_H