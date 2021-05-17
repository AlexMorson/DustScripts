class TilePalette : Tool, IToolSelectListener, IToolStepListener, IToolDrawListener, IToolEditorLoadListener
{
	
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

		for (int i=0; i<24; ++i)
		{
			Button@ b = Button(ui, "" + i);
			b.name = "" + i;
			b.selectable = true;
			button_group.add(b);
			window.add_child(b);
		}

		ui.add_child(window);
		window.fit_to_contents(true);
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
		if (mouse.left_press)
		{

		}
	}
	
	void tool_draw(Tool@ tool, const float sub_frame) override
	{
	}
	
	// ///////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////
	
	private void on_button_select(EventInfo@ event)
	{
		if(@event.target == null)
			return;
		
		puts(event.target.name);
	}
}
