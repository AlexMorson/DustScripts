class TilePalette : Tool, IToolSelectListener, IToolStepListener, IToolDrawListener, IToolEditorLoadListener
{
	
	private Mouse@ mouse;
	private Window@ palette;
	
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

		@palette = Window(ui, "Tile Palette", false);

		GridLayout@ grid = GridLayout(ui, 4);
		grid.row_spacing = 0;
		grid.column_spacing = 0;
		@palette.layout = grid;

		ButtonGroup@ button_group = ButtonGroup(ui);

		for (int i=0; i<24; ++i)
		{
			Button@ b = Button(ui, "" + i);
			b.selectable = true;
			button_group.add(b);
			palette.add_child(b);
		}

		ui.add_child(palette);

		palette.show();
		palette.fit_to_contents(true);
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
		puts("ToolSelect");
	}
	
	void tool_deselect(Tool@ tool) override
	{
		puts("ToolDeselect");
	}
	
	void tool_step(Tool@ tool) override
	{
	}
	
	void tool_draw(Tool@ tool, const float sub_frame) override
	{
	}
}
