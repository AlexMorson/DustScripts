#include "../../../../lib/tiles/common.cpp"

#include "TileEmbeds.cpp"
#include "TilesWindow.cpp"
#include "ShapeWindow.cpp"

const string EMBED_spr_icon_shape_tool = SPRITES_BASE + "icon_shape_tool.png";


class ShapeTool : Tool
{
	
	private Mouse@ mouse;
	private TilesWindow@ tiles_window;
	private ShapeWindow@ shape_window;
	
	ShapeTool(AdvToolScript@ script)
	{
		super(script, 'Shape Tool');

		init_shortcut_key(VK::W);
	}

	void create(ToolGroup@ group) override
	{
		Tool::create(group);
		
		set_icon(SPRITE_SET, "icon_shape_tool", 40, 40);
	}
	
	void on_init() override
	{
		@mouse = @script.mouse;

		UI@ ui = script.ui;

		@tiles_window = TilesWindow(ui);
		tiles_window.x = 90;
		tiles_window.y = 70;
		ui.add_child(tiles_window);
		script.window_manager.register_element(tiles_window);

		@shape_window = ShapeWindow(ui);
		shape_window.x = 460;
		shape_window.y = 70;
		ui.add_child(shape_window);
		script.window_manager.register_element(shape_window);
	}

	void build_sprites(message@ msg) override
	{
		build_sprite(msg, "icon_shape_tool");
		build_tile_sprites(msg);
	}

	private void set_tile_at_mouse(bool solid)
	{
		const int layer = script.editor.get_selected_layer();
		if (layer <= 5 or layer > 20) return;

		float mx, my;
		script.transform(mouse.x, mouse.y, 19, layer, mx, my);
		const int tile_x = int(floor(mx / 48));
		const int tile_y = int(floor(my / 48));

		script.g.set_tile(
			tile_x,
			tile_y,
			layer,
			solid,
			shape_window.tile_shape,
			tiles_window.sprite_set,
			tiles_window.sprite_tile,
			tiles_window.sprite_palette
		);
	}

	private void pick_tile_at_mouse()
	{
		for (int layer=20; layer>=6; --layer)
		{
			if (not script.editor.get_layer_visible(layer))
				continue;

			const float mx = script.g.mouse_x_world(0, layer);
			const float my = script.g.mouse_y_world(0, layer);

			const int tile_x = int(floor(mx / 48));
			const int tile_y = int(floor(my / 48));

			tileinfo@ tile = script.g.get_tile(tile_x, tile_y, layer);

			float _;
			if (tile.solid() and point_in_tile(mx, my, tile_x, tile_y, tile.type(), _, _, layer))
			{
				script.editor.set_selected_layer(layer);
				tiles_window.select_tile(tile.sprite_set(), tile.sprite_tile());
				tiles_window.sprite_palette = tile.sprite_palette();
				shape_window.tile_shape = tile.type();
				break;
			}
		}
	}

	private void change_layer(int delta)
	{
		int layer = script.editor.get_selected_layer() + delta;
		if (layer <= 5 or layer > 20) return;

		if (layer == 18)
			layer += sign(delta);

		script.editor.set_selected_layer(layer);
	}
	
	// //////////////////////////////////////////////////////////
	// Callbacks
	// //////////////////////////////////////////////////////////
	
	Tool@ on_shortcut_key() override
	{
		Tool@ tiles = script.get_tool('Tiles');
		return tiles.selected ? cast<Tool>(this) : tiles;
	}
	
	protected void on_select_impl()
	{
		script.editor.hide_panels_gui(true);

		tiles_window.show();
		shape_window.show();
	}
	
	protected void on_deselect_impl()
	{
		script.editor.hide_panels_gui(false);

		tiles_window.hide();
		shape_window.hide();
	}
	
	protected void step_impl() override
	{
		if (script.mouse_in_scene and not script.space)
		{
			if (mouse.left_down) set_tile_at_mouse(true);
			if (mouse.right_down) set_tile_at_mouse(false);
			if (mouse.middle_press) pick_tile_at_mouse();

			if (script.ctrl and mouse.scroll != 0) change_layer(mouse.scroll);
		}

		if (script.scene_focus)
		{
			if (script.key_repeat_gvb(GVB::LeftArrow )) shape_window.move_selection(-1,  0);
			if (script.key_repeat_gvb(GVB::RightArrow)) shape_window.move_selection( 1,  0);
			if (script.key_repeat_gvb(GVB::UpArrow   )) shape_window.move_selection( 0, -1);
			if (script.key_repeat_gvb(GVB::DownArrow )) shape_window.move_selection( 0,  1);
		}
	}

	protected void draw_impl(const float sub_frame) override
	{
		if (script.space or not script.mouse_in_scene) return;

		const int layer = script.editor.get_selected_layer();
		if (layer <= 5 or layer > 20) return;

		float mx, my;
		script.transform(mouse.x, mouse.y, 19, layer, mx, my);
		mx = 48 * floor(mx / 48);
		my = 48 * floor(my / 48);
		script.transform(mx, my, layer, 19, mx, my);

		float sx, sy;
		script.transform_size(1, 1, layer, 19, sx, sy);

		draw_tile_shape(shape_window.tile_shape, script.g, 22, 22, mx, my, sx, sy, Settings::HoveredFillColour, Settings::HoveredLineColour);
	}
}

void draw_tile_shape(int shape, scene@ g, int layer, int sub_layer, float x, float y, float scale_x, float scale_y, uint fill, uint outline)
{
	float x1, y1, x2, y2, x3, y3, x4, y4;

	get_tile_quad(shape, x1, y1, x2, y2, x3, y3, x4, y4);
	g.draw_quad_world(
		layer, sub_layer, false,
		x + scale_x * x1, y + scale_y * y1,
		x + scale_x * x2, y + scale_y * y2,
		x + scale_x * x3, y + scale_y * y3,
		x + scale_x * x4, y + scale_y * y4,
		fill, fill, fill, fill
	);

	for (int edge=0; edge<4; ++edge)
	{
		if (get_edge_points(shape, edge, x1, y1, x2, y2))
		{
			g.draw_line_world(
				layer, sub_layer,
				x + scale_x * x1, y + scale_y * y1,
				x + scale_x * x2, y + scale_y * y2,
				2, outline
			);
		}
	}
}
