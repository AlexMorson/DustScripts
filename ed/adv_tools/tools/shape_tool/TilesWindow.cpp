const array<array<int>> TILE_COUNTS = {
	{3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1},
    {3, 3, 3, 2, 2, 2, 2, 2, 3, 1, 3, 3, 1},
    {4, 2, 2, 1, 1, 1},
    {4, 2, 1, 4, 4, 4, 4, 4, 1},
    {6, 1}
};


class PaletteMenu : Container
{
	private int _sprite_set = 1;
	private int _sprite_tile = 1;
	private int _sprite_palette = 1;

    int sprite_palette { get const { return _sprite_palette; } }

	private ButtonGroup@ button_group;

	PaletteMenu(UI@ ui)
	{
		super(ui);

		@layout = FlowLayout(ui, FlowDirection::Row);
		width = 72 + 2 * ui.style.spacing;

		@button_group = ButtonGroup(ui, false);
		button_group.select.on(EventCallback(on_palette_select));

		create_ui();
	}

	void select_tile(int sprite_set, int sprite_tile)
	{
		if (sprite_set == _sprite_set and sprite_tile == _sprite_tile) return;

		_sprite_set = sprite_set;
		_sprite_tile = sprite_tile;
		_sprite_palette = int(min(_sprite_palette, TILE_COUNTS[sprite_set-1][sprite_tile-1]));

		create_ui();
	}

	private void create_ui()
	{
		button_group.clear();
		clear();

		for (int sprite_palette=1; sprite_palette<=TILE_COUNTS[_sprite_set-1][_sprite_tile-1]; ++sprite_palette)
		{
			Image@ t = Image(ui, SPRITE_SET, "tile_" + _sprite_set + "_" + _sprite_tile + "_" + sprite_palette, 96, 96);
			t.width = 72;
			t.height = 72;
			t.sizing = ImageSize::None;

			Button@ b = Button(ui, t);
			b.name = "" + sprite_palette;
			b.width = 72;
			b.height = 72;
			b.selectable = true;

			if (sprite_palette == _sprite_palette)
				b.selected = true;

			button_group.add(b);
			add_child(b);
		}
	}

	private void on_palette_select(EventInfo@ event)
	{
		if (event.target is null) return;

		int sprite_palette = parseInt(event.target.name);
		_sprite_palette = sprite_palette;
	}
}


class TilesWindow : Window
{

	private int _sprite_set = 1;
	private int _sprite_tile = 1;

    int sprite_set { get const { return _sprite_set; } }
    int sprite_tile { get const { return _sprite_tile; } }
    int sprite_palette { get const { return palette_menu.sprite_palette; } }

	private PaletteMenu@ palette_menu;

	TilesWindow(UI@ ui)
	{
		super(ui, "Tile", false);

		@layout = FlowLayout(ui, FlowDirection::Column);

		ScrollView@ tiles_view = ScrollView(ui);
		tiles_view.x = 0;
		tiles_view.y = 0;
		tiles_view.width = 3 * 72 + 5 * ui.style.spacing + ui.style.default_scrollbar_size;
		tiles_view.height = 6 * 72 + 7 * ui.style.spacing + 12 * EPSILON; // was having trouble with the bottom row
		tiles_view.scroll_amount = 72 + ui.style.spacing - EPSILON;

		Container@ tiles_container = tiles_view.content;

		GridLayout@ grid = GridLayout(ui, 3);
		@tiles_container.layout = grid;

		ButtonGroup@ button_group = ButtonGroup(ui, false);
		button_group.select.on(EventCallback(on_tile_select));

		for (uint sprite_set=1; sprite_set<=TILE_COUNTS.size(); ++sprite_set)
		{
			for (uint sprite_tile=1; sprite_tile<=TILE_COUNTS[sprite_set-1].size(); ++sprite_tile)
			{
				Image@ t = Image(ui, SPRITE_SET, "tile_" + sprite_set + "_" + sprite_tile + "_1", 96, 96);
				t.width = 72;
				t.height = 72;
				t.sizing = ImageSize::None;

				Button@ b = Button(ui, t);
				b.name = sprite_set + "_" + sprite_tile;
				b.width = 72;
				b.height = 72;
				b.selectable = true;

				button_group.add(b);
				tiles_container.add_child(b);

				if (sprite_set == 1 and sprite_tile == 1)
					b.selected = true;
			}
		}

		@palette_menu = PaletteMenu(ui);

		add_child(tiles_view);
		add_child(palette_menu);

		fit_to_contents(true);
	}
	
	// ///////////////////////////////////////////
	// Events
	// ///////////////////////////////////////////
	
	private void on_tile_select(EventInfo@ event)
	{
		if (event.target is null) return;

		array<string> sprite_info = event.target.name.split("_");

		int sprite_set = parseInt(sprite_info[0]);
		int sprite_tile = parseInt(sprite_info[1]);

		if (sprite_set == _sprite_set and sprite_tile == _sprite_tile) return;

		_sprite_set = sprite_set;
		_sprite_tile = sprite_tile;

		palette_menu.select_tile(sprite_set, sprite_tile);
	}
}
