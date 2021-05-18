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
		if (script.space or not script.mouse_in_scene) return;

		if (not mouse.left_down and not mouse.right_down) return;

		const int layer = script.editor.get_selected_layer();

		float mx, my;
		script.transform(mouse.x, mouse.y, 19, layer, mx, my);
		int tile_x = int(floor(mx / 48));
		int tile_y = int(floor(my / 48));

		if (mouse.left_down)
		{
			script.g.set_tile(
				tile_x,
				tile_y,
				layer,
				true,
				shape_window.tile_shape,
				tiles_window.sprite_set,
				tiles_window.sprite_tile,
				tiles_window.sprite_palette
			);
		}

		if (mouse.right_down)
		{
			script.g.set_tile(
				tile_x,
				tile_y,
				layer,
				false,
				shape_window.tile_shape,
				tiles_window.sprite_set,
				tiles_window.sprite_tile,
				tiles_window.sprite_palette
			);
		}
	}

	protected void draw_impl(const float sub_frame) override
	{
		if (script.space or not script.mouse_in_scene) return;

		const int layer = script.editor.get_selected_layer();

		if (layer <= 5) return;

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
