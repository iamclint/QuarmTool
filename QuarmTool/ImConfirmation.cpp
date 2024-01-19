#include "ImConfirmation.h"
#include "ImGui/imgui.h"

bool ImConfirmation::confirm(const std::string& title_, const std::string& message_, std::function<void(bool)> callback_)
{
	if (!this->wait_confirm)
	{
		title = title_;
		message = message_;
		callback = callback_;
		this->wait_confirm = true;
		this->do_popup = true;
		return true;
	}
	return false;
}
void ImConfirmation::draw()
{
	if (this->do_popup)
	{
		ImGui::OpenPopup((this->title + "##confirm").c_str());
		this->do_popup = false;
	}
	if (ImGui::BeginPopupModal((this->title + "##confirm").c_str(), 0, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(this->message.c_str());
		if (ImGui::BeginTable((this->title + "##confirmbtn").c_str(), 2, ImGuiTableFlags_SizingFixedSame))
		{
			ImGui::TableNextColumn();
			if (ImGui::Button("Yes"))
			{
				this->wait_confirm = false;
				callback(true);
				ImGui::CloseCurrentPopup();
			}
			ImGui::TableNextColumn();
			if (ImGui::Button("No"))
			{
				this->wait_confirm = false;
				callback(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndTable();
		}
	}
}