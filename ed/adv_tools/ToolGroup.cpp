#include 'tools/Tool.cpp';
#include '../../lib/ui3/elements/Button.cpp';
#include '../../lib/ui3/elements/Image.cpp';
#include '../../lib/ui3/utils/ButtonGroup.cpp';

class ToolGroup
{
	
	AdvToolScript@ script;
	string name;
	array<Tool@> tools;
	Button@ button;
	Image@ icon;
	
	private ButtonGroup@ button_group;
	private Tool@ current_tool;
	private array<Tool@> selectable_tools;
	private int num_selectable_tools;
	
	private PopupOptions@ popup;
	private Container@ popup_content;
	
	ToolGroup()
	{
		
	}
	
	void create(AdvToolScript@ script, const string name, ButtonGroup@ button_group)
	{
		@this.script = script;
		this.name = name;
		@this.button_group = button_group;
		
		@button = create_button(name, -1, '', '');
		button.mouse_enter.on(EventCallback(on_button_enter));
		
		@icon = cast<Image@>(button.content);
	}
	
	void init_ui()
	{
		for(uint i = 0; i < tools.length(); i++)
		{
			Tool@ tool = @tools[i];
			
			if(@tool.toolbar_button == null)
				continue;
			
			Image@ icon = cast<Image@>(tool.toolbar_button.content);
			
			icon.set_sprite(tool.icon_sprite_set,
				tool.icon_sprite_name, tool.icon_width, tool.icon_height,
				tool.icon_offset_x, tool.icon_offset_y);
		}
	}
	
	void on_init()
	{
		for(uint i = 0; i < tools.length(); i++)
		{
			tools[i].on_init();
		}
	}
	
	Button@ create_button(const string name, const int shortcut_key,
		const string sprite_set, const string sprite_name,
		const float width=-1, const float height=-1,
		const float offset_x=0, const float offset_y=0)
	{
		Image@ icon = Image(script.ui, sprite_set, sprite_name, width, height, offset_x, offset_y);
		icon.width = Settings::ToolbarIconSize;
		icon.height = Settings::ToolbarIconSize;
		
		Button@ button = Button(script.ui, icon);
		button.name = name;
		button.selectable = true;
		button.fit_to_contents();
		@button.group = button_group;
		@button.tooltip = PopupOptions(script.ui, get_tooltip(name, shortcut_key), false, PopupPosition::Below);
		
		return button;
	}
	
	void add_tool(Tool@ tool)
	{
		tools.insertLast(@tool);
		
		if(tool.selectable)
		{
			selectable_tools.insertLast(tool);
			num_selectable_tools++;
		}
		
		if(num_selectable_tools > 1)
		{
			if(num_selectable_tools == 2)
			{
				create_popup();
			}
			else
			{
				create_popup_button(tool);
			}
		}
		
		tool.create(script, this);
		
		if(@current_tool == null)
		{
			set_tool(tool);
		}
	}
	
	void set_tool(Tool@ tool)
	{
		@current_tool = tool;
		
		button.tooltip.content_string = get_tooltip(tool.name, tool.shortcut_key);
		button.name = tool.name;
		
		icon.set_sprite(tool.icon_sprite_set, tool.icon_sprite_name, tool.icon_width, tool.icon_height, tool.icon_offset_x, tool.icon_offset_y);
		icon.width = Settings::ToolbarIconSize;
		icon.height = Settings::ToolbarIconSize;
		
		update_popup_buttons();
	}
	
	Tool@ get_next_selectable_tool(Tool@ tool, const int dir=1)
	{
		if(num_selectable_tools == 0)
			return null;
		
		if(@tool == null)
			return selectable_tools[0];
		
		int index = selectable_tools.findByRef(tool);
		
		if(index == -1)
			return selectable_tools[0];
		
		index += dir >= 0 ? 1 : -1;
		
		if(index < 0 || index >= num_selectable_tools)
			return null;
		
		return selectable_tools[index];
	}
	
	Tool@ get_first_selectable_tool()
	{
		return num_selectable_tools > 0
			? selectable_tools[0] : null;
	}
	
	Tool@ get_last_selectable_tool()
	{
		return num_selectable_tools > 0
			? selectable_tools[num_selectable_tools - 1] : null;
	}
	
	void on_select()
	{
		button.selected = true;
		button.override_alpha = 1;
	}
	
	void on_deselect()
	{
		button.selected = false;
		button.override_alpha = -1;
	}
	
	private void create_popup()
	{
		if(@popup != null)
			return;
		
		@popup_content = Container(script.ui);
		popup_content.mouse_self = false;
		popup_content.fit_to_contents(true);

		@popup = PopupOptions(script.ui, popup_content, true, PopupPosition::Below, PopupTriggerType::Manual);
		popup.mouse_self = false;
		popup.padding = script.ui.style.spacing;
		popup.spacing = 0;
		popup.show.on(EventCallback(on_popup_show));
		popup.hide.on(EventCallback(on_popup_hide));
		
		for(uint i = 0; i < tools.length(); i++)
		{
			create_popup_button(tools[i]);
		}
	}
	
	private void create_popup_button(Tool@ tool)
	{
		if(!tool.selectable)
			return;
		
		@tool.toolbar_button = create_button(tool.name, tool.shortcut_key,
			tool.icon_sprite_set, tool.icon_sprite_name, tool.icon_width, tool.icon_height,
			tool.icon_offset_x, tool.icon_offset_y);
		popup_content.add_child(tool.toolbar_button);
	}
	
	private void update_popup_buttons()
	{
		float y = 0;
		
		for(uint i = 0; i < tools.length(); i++)
		{
			Tool@ tool = tools[i];
			Button@ button = tool.toolbar_button;
			
			if(@button == null)
				return;
			
			button.visible = @tool != @current_tool;
			
			if(!button.visible)
				continue;
			
			button.y = y;
			y += button.height + script.ui.style.spacing;
		}
		
		popup_content.fit_to_contents(true);
	}
	
	private string get_tooltip(const string name, const int shortcut_key)
	{
		if(shortcut_key == -1)
			return name;
		
		string s = ' ';
		s[0] = shortcut_key;
		
		return name + ' [' + s + ']';
	}
	
	// //////////////////////////////////////////////////////////
	// Events
	// //////////////////////////////////////////////////////////
	
	void on_button_enter(EventInfo@ event)
	{
		if(tools.length() <= 1)
			return;
		
		update_popup_buttons();
		script.ui.show_tooltip(popup, event.target);
	}
	
	void on_popup_show(EventInfo@ event)
	{
		script.track_tool_group_popups(true);
	}
	
	void on_popup_hide(EventInfo@ event)
	{
		script.track_tool_group_popups(false);
	}
	
}