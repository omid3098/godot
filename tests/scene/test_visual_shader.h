/**************************************************************************/
/*  test_visual_shader.h                                                  */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "scene/resources/visual_shader.h"
#include "scene/resources/visual_shader_nodes.h"

#include "tests/test_macros.h"

namespace TestVisualArray {

TEST_CASE("[SceneTree][VisualShader] Object creation and parameter") {
	Ref<VisualShader> vs = memnew(VisualShader);
	CHECK(vs.is_valid());

	CHECK(vs->get_mode() == Shader::MODE_SPATIAL);

	for (int i = 1; i < Shader::MODE_MAX; i++) {
		vs->set_mode((Shader::Mode)i);
		CHECK(vs->get_mode() == i);
	}
}

TEST_CASE("[SceneTree][VisualShader] Testing VisualShaderNodes") {
	SUBCASE("Testing Node Creation") {
		Ref<VisualShader> vs = memnew(VisualShader);
		CHECK(vs.is_valid());

		for (int i = 0; i < VisualShader::TYPE_MAX; i++) {
			Ref<VisualShaderNode> vsn = memnew(VisualShaderNodeInput);
			CHECK(vsn.is_valid());
			vs->add_node(VisualShader::Type(i), vsn, Vector2(1, 10), i + 2);
			CHECK(vs->get_node(VisualShader::Type(i), i + 2) == vsn);
		}

		ERR_PRINT_OFF;

		// Testing for Invalid entries.
		Ref<VisualShaderNode> vsn5 = memnew(VisualShaderNodeInput);
		Ref<VisualShaderNode> vsn6 = memnew(VisualShaderNodeInput);
		CHECK(vsn6.is_valid());
		CHECK(vsn5.is_valid());

		vs->add_node(VisualShader::TYPE_SKY, vsn5, Vector2(1, 10), 0);
		CHECK_FALSE(vs->get_node(VisualShader::TYPE_SKY, 0) == vsn6);
		vs->add_node(VisualShader::TYPE_MAX, vsn6, Vector2(1, 10), 7);
		CHECK_FALSE(vs->get_node(VisualShader::TYPE_SKY, 7) == vsn6);

		ERR_PRINT_ON;
	}

	SUBCASE("Testing VisualShaderNode position getter and setter") {
		Ref<VisualShader> vs = memnew(VisualShader);
		CHECK(vs.is_valid());

		Ref<VisualShaderNode> vsn1 = memnew(VisualShaderNodeInput);
		CHECK(vsn1.is_valid());
		vs->add_node(VisualShader::TYPE_COLLIDE, vsn1, Vector2(0, 0), 3);
		CHECK(vs->get_node_position(VisualShader::TYPE_COLLIDE, 3) == Vector2(0, 0));
		vs->set_node_position(VisualShader::TYPE_COLLIDE, 3, Vector2(1, 1));
		CHECK(vs->get_node_position(VisualShader::TYPE_COLLIDE, 3) == Vector2(1, 1));

		Ref<VisualShaderNode> vsn2 = memnew(VisualShaderNodeInput);
		CHECK(vsn2.is_valid());
		vs->add_node(VisualShader::TYPE_FOG, vsn2, Vector2(1, 2), 4);
		CHECK(vs->get_node_position(VisualShader::TYPE_FOG, 4) == Vector2(1, 2));
		vs->set_node_position(VisualShader::TYPE_FOG, 4, Vector2(2, 2));
		CHECK(vs->get_node_position(VisualShader::TYPE_FOG, 4) == Vector2(2, 2));
	}

	SUBCASE("Testing VisualShaderNode ID") {
		Ref<VisualShader> vs = memnew(VisualShader);
		CHECK(vs.is_valid());

		for (int i = 0; i < VisualShader::TYPE_MAX; i++) {
			Ref<VisualShaderNode> vsn = memnew(VisualShaderNodeInput);
			CHECK(vsn.is_valid());
			vs->add_node(VisualShader::Type(i), vsn, Vector2(1, 10), i + 2);
			CHECK(vs->get_valid_node_id(VisualShader::Type(i)) - 1 == i + 2);
		}
	}

	SUBCASE("Testing remove and replace VisualShaderNode") {
		Ref<VisualShader> vs = memnew(VisualShader);
		CHECK(vs.is_valid());

		ERR_PRINT_OFF;

		for (int i = 0; i < VisualShader::TYPE_MAX; i++) {
			Ref<VisualShaderNode> vsn = memnew(VisualShaderNodeInput);
			CHECK(vsn.is_valid());
			vs->add_node(VisualShader::Type(i), vsn, Vector2(1, 10), i + 2);
			CHECK(vs->get_node(VisualShader::Type(i), i + 2) == vsn);
			vs->remove_node(VisualShader::Type(i), i + 2);
			CHECK_FALSE(vs->get_node(VisualShader::Type(i), i + 2) == vsn);
		}

		ERR_PRINT_ON;
	}
}

TEST_CASE("[SceneTree][VisualShader] Testing Varyings") {
	Ref<VisualShader> vs = memnew(VisualShader);

	vs->add_varying("Test1", VisualShader::VARYING_MODE_FRAG_TO_LIGHT, VisualShader::VARYING_TYPE_TRANSFORM);
	CHECK(vs->has_varying("Test1") == true);

	vs->add_varying("Test2", VisualShader::VARYING_MODE_VERTEX_TO_FRAG_LIGHT, VisualShader::VARYING_TYPE_VECTOR_2D);
	CHECK(vs->has_varying("Test2"));

	CHECK_FALSE(vs->has_varying("Does_not_exits"));
	ERR_PRINT_OFF;
	vs->add_varying("Test3", VisualShader::VARYING_MODE_MAX, VisualShader::VARYING_TYPE_INT);
	CHECK_FALSE(vs->has_varying("Test3"));

	vs->add_varying("Test4", VisualShader::VARYING_MODE_FRAG_TO_LIGHT, VisualShader::VARYING_TYPE_MAX);
	CHECK_FALSE(vs->has_varying("Test4"));
	ERR_PRINT_ON;
}

TEST_CASE("[SceneTree][VisualShader] Group editing functionality") {
	SUBCASE("Test basic group node creation and management") {
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		// Create a group node
		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		CHECK(group_node.is_valid());

		// Add the group node to the visual shader
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		print_line("[TEST] Adding group node with ID: ", group_id);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Verify group node was added
		Vector<int> nodes = vs->get_node_list(VisualShader::TYPE_FRAGMENT);
		print_line("[TEST] Total nodes after adding group: ", nodes.size());
		CHECK(nodes.size() == 2); // Output + Group

		Ref<VisualShaderNode> retrieved_node = vs->get_node(VisualShader::TYPE_FRAGMENT, group_id);
		CHECK(retrieved_node.is_valid());
		CHECK(Object::cast_to<VisualShaderNodeGroup>(retrieved_node.ptr()) != nullptr);
	}

	SUBCASE("Test group internal node ID generation") {
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Test consecutive ID generation - CRITICAL for interface node creation
		int internal_id1 = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		int internal_id2 = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		int internal_id3 = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);

		print_line("[TEST] Generated consecutive IDs: ", internal_id1, ", ", internal_id2, ", ", internal_id3);

		// Each call must return a unique ID
		CHECK(internal_id1 != internal_id2);
		CHECK(internal_id2 != internal_id3);
		CHECK(internal_id1 != internal_id3);

		// IDs should be sequential
		CHECK(internal_id2 == internal_id1 + 1);
		CHECK(internal_id3 == internal_id2 + 1);

		CHECK(internal_id1 >= 0);
		CHECK(internal_id2 >= 0);
		CHECK(internal_id3 >= 0);
	}

	SUBCASE("Test adding interface nodes to group") {
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Simulate the exact interface node creation process from _add_group_interface_nodes
		print_line("[TEST] === Simulating interface node creation ===");

		// Generate IDs for input and output nodes (like the editor does)
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);

		print_line("[TEST] Generated input ID: ", input_id, ", output ID: ", output_id);

		// CRITICAL: IDs must be different!
		CHECK(input_id != output_id);
		CHECK(output_id == input_id + 1);

		// Add group input node
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		input_node->set_input_name("input");
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);
		print_line("[TEST] Successfully added input node");

		// Add group output node
		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		// Initialize the output node with proper shader mode and type
		output_node->set_shader_mode(vs->get_mode());
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);
		print_line("[TEST] Successfully added output node");

		// Verify nodes were added
		Vector<int> internal_nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		print_line("[TEST] Internal nodes count: ", internal_nodes.size());
		CHECK(internal_nodes.size() == 2);

		// Verify we can retrieve the nodes and they're the correct types
		Ref<VisualShaderNode> retrieved_input = group_node->get_internal_node(VisualShader::TYPE_FRAGMENT, input_id);
		Ref<VisualShaderNode> retrieved_output = group_node->get_internal_node(VisualShader::TYPE_FRAGMENT, output_id);

		CHECK(retrieved_input.is_valid());
		CHECK(retrieved_output.is_valid());

		// Verify correct node types
		VisualShaderNodeInput *input_cast = Object::cast_to<VisualShaderNodeInput>(retrieved_input.ptr());
		VisualShaderNodeOutput *output_cast = Object::cast_to<VisualShaderNodeOutput>(retrieved_output.ptr());

		CHECK(input_cast != nullptr);
		CHECK(output_cast != nullptr);

		print_line("[TEST] Interface nodes successfully created and verified!");
	}

	SUBCASE("Test ID conflict detection and prevention") {
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Add first node
		Ref<VisualShaderNodeColorConstant> color_node1 = memnew(VisualShaderNodeColorConstant);
		int id1 = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node1, Vector2(0, 0), id1);

		// Add second node - should get different ID
		Ref<VisualShaderNodeColorConstant> color_node2 = memnew(VisualShaderNodeColorConstant);
		int id2 = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);

		print_line("[TEST] ID1: ", id1, ", ID2: ", id2);
		CHECK(id1 != id2);

		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node2, Vector2(100, 0), id2);

		// Verify both nodes exist with unique IDs
		Vector<int> internal_nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		CHECK(internal_nodes.size() == 2);
		CHECK(internal_nodes.has(id1));
		CHECK(internal_nodes.has(id2));

		// Test manual ID conflict handling
		ERR_PRINT_OFF;
		Ref<VisualShaderNodeColorConstant> color_node3 = memnew(VisualShaderNodeColorConstant);
		// Try to add with same ID as existing node - should fail
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node3, Vector2(200, 0), id1);
		ERR_PRINT_ON;

		// Should still only have two nodes
		Vector<int> nodes_after_conflict = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		CHECK(nodes_after_conflict.size() == 2);
	}

	SUBCASE("Test adding regular nodes to group after interface setup") {
		print_line("[TEST] === Testing adding regular nodes to group ===");

		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// First add interface nodes
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);

		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		// Initialize the output node with proper shader mode and type
		output_node->set_shader_mode(vs->get_mode());
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);

		// Now add a user node (simulating what the editor should do)
		print_line("[TEST] Adding user node to group...");
		Ref<VisualShaderNodeColorConstant> user_node = memnew(VisualShaderNodeColorConstant);

		// This should use the group's ID generation, NOT the main shader's
		int user_node_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		print_line("[TEST] Generated ID for user node: ", user_node_id);

		// Ensure the generated ID doesn't conflict with existing nodes
		CHECK(user_node_id != input_id);
		CHECK(user_node_id != output_id);

		// Add the node to the group's internal graph
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, user_node, Vector2(0, 100), user_node_id);

		// Verify the node was added
		Vector<int> nodes = group_node->get_internal_node_list(VisualShader::TYPE_FRAGMENT);
		print_line("[TEST] Total nodes after adding user node: ", nodes.size());
		CHECK(nodes.size() == 3); // Input + Output + UserNode
		CHECK(nodes.has(user_node_id));

		Ref<VisualShaderNode> retrieved = group_node->get_internal_node(VisualShader::TYPE_FRAGMENT, user_node_id);
		CHECK(retrieved.is_valid());
		CHECK(Object::cast_to<VisualShaderNodeColorConstant>(retrieved.ptr()) != nullptr);

		print_line("[TEST] Successfully added user node to group!");
	}

	SUBCASE("Test group internal connections") {
		Ref<VisualShader> vs = memnew(VisualShader);
		vs->set_mode(Shader::MODE_CANVAS_ITEM);

		Ref<VisualShaderNodeGroup> group_node = memnew(VisualShaderNodeGroup);
		int group_id = vs->get_valid_node_id(VisualShader::TYPE_FRAGMENT);
		vs->add_node(VisualShader::TYPE_FRAGMENT, group_node, Vector2(100, 100), group_id);

		// Add nodes to group
		Ref<VisualShaderNodeInput> input_node = memnew(VisualShaderNodeInput);
		int input_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, input_node, Vector2(-200, 0), input_id);

		Ref<VisualShaderNodeOutput> output_node = memnew(VisualShaderNodeOutput);
		// Initialize the output node with proper shader mode and type
		output_node->set_shader_mode(vs->get_mode());
		output_node->set_shader_type(VisualShader::TYPE_FRAGMENT);
		int output_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, output_node, Vector2(200, 0), output_id);

		Ref<VisualShaderNodeColorConstant> color_node = memnew(VisualShaderNodeColorConstant);
		int color_id = group_node->get_valid_internal_node_id(VisualShader::TYPE_FRAGMENT);
		group_node->add_internal_node(VisualShader::TYPE_FRAGMENT, color_node, Vector2(0, 0), color_id);

		// Test connecting nodes within the group
		bool can_connect = group_node->can_connect_internal_nodes(
				VisualShader::TYPE_FRAGMENT,
				color_id, 0, // color output
				output_id, 0 // output input
		);

		print_line("[TEST] Can connect color to output: ", can_connect);
		CHECK(can_connect);

		// Make the connection
		group_node->connect_internal_nodes(
				VisualShader::TYPE_FRAGMENT,
				color_id, 0,
				output_id, 0);

		// Verify connection exists
		List<VisualShader::Connection> connections;
		group_node->get_internal_node_connections(VisualShader::TYPE_FRAGMENT, &connections);

		print_line("[TEST] Internal connections count: ", connections.size());
		CHECK(connections.size() == 1);

		VisualShader::Connection &conn = connections.front()->get();
		CHECK(conn.from_node == color_id);
		CHECK(conn.from_port == 0);
		CHECK(conn.to_node == output_id);
		CHECK(conn.to_port == 0);
	}
}

} //namespace TestVisualArray
