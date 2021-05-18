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
		/*
		if (mouse.left_down and not script.space)
		{
			int tile_x = int(floor(mouse.x / 48));
			int tile_y = int(floor(mouse.y / 48));
			script.g.set_tile(tile_x, tile_y, script.editor.get_selected_layer(), true, current_shape, 1, 1, 1);
		}
		*/
	}

	protected void draw_impl(const float sub_frame) override
	{

	}
}
