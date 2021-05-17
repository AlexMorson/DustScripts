#include "../../../lib/ui3/elements/TileShapeGraphic.cpp"

class TilePalette : Tool, IToolSelectListener, IToolStepListener, IToolDrawListener, IToolEditorLoadListener
{
	
	private int current_shape = -1;
	private Mouse@ mouse;
	private Window@ window;
	
	TilePalette(AdvToolScript@ script)
	{
		super(script, 'Tile Palette');
		
		selectable = false;
	}
	
	void on_init() override
	{
		Tool@ tool = group.script.get_tool('Tiles');
		
		if(@tool != null)
		{
			tool.register_editor_load_listener(this);
			tool.register_select_listener(this);
			tool.register_step_listener(this);
			tool.register_draw_listener(this);
		}

		@mouse = @script.mouse;

		UI@ ui = script.ui;

		@window = Window(ui, "Tile Palette"); // , false);

		GridLayout@ grid = GridLayout(ui, 4);
		grid.row_spacing = 0;
		grid.column_spacing = 0;
		@window.layout = grid;

		ButtonGroup@ button_group = ButtonGroup(ui);
		button_group.select.on(EventCallback(on_button_select));

		const array<int> tile_map = {
			 8, 16,  0,  0,
			 7, 15,  0,  0,
			11,  3, 20, 17,
			12,  4, 19, 18,
			10,  9,  1,  2,
			 6,  5, 13, 14
		};

		for (int i=0; i<24; ++i)
		{
			int shape = tile_map[i];
			TileShapeGraphic@ t = TileShapeGraphic(ui, shape);
			Button@ b = Button(ui, t);
			b.name = "" + shape;
			b.width = 48;
			b.height = 48;
			b.selectable = true;
			button_group.add(b);
			window.add_child(b);
		}

		ui.add_child(window);
		window.fit_to_contents(true);

		script.window_manager.register_element(window);
	}
	
	// //////////////////////////////////////////////////////////
	// Tool Callbacks
	// //////////////////////////////////////////////////////////
	
	void tool_editor_loaded(Tool@ tool) override
	{
		puts("EditorLoad");
	}
	
	void tool_editor_unloaded(Tool@ tool) override
	{
		puts("EditorUnload");
	}
	
	void tool_select(Tool@ tool) override
	{
		window.show();
	}
	
	void tool_deselect(Tool@ tool) override
	{
		window.hide();
	}
	
	void tool_step(Tool@ tool) override
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
	
	void tool_draw(Tool@ tool, const float sub_frame) override
	{
	}
	
	// ///////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////
	
	private void on_button_select(EventInfo@ event)
	{
		if (event.target is null) current_shape = -1;
		else current_shape = parseInt(event.target.name);
	}
}
