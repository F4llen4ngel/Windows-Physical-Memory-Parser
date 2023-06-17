#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <format>

#include "FileBrowser/ImGuiFileBrowser.h"
#include "GUI.h"

#include "memory.h"


int main()
{

	GUI gui;
	imgui_addons::ImGuiFileBrowser file_dialog;

	bool showguidemo = false;
	bool showplotdemo = false;

	bool open = false;
	std::string path_to_file = "";
    bool fileIsAnalyzed = false;

    std::ifstream file;
    uint64_t systemKProcessAddress = 0xdeadbeef;
    std::vector<Process> processList;

	while (!gui.WindowShouldClose())
	{
		gui.Prepare();
		{
			static bool opt_fullscreen = true;
			static bool opt_padding = false;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->WorkPos);
				ImGui::SetNextWindowSize(viewport->WorkSize);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}
			else
			{
				dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
			}

			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			if (!opt_padding)
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", (bool*)0, window_flags);
			if (!opt_padding)
				ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("DockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open file")) { 
						open = true;
					}
					ImGui::Separator();
					if (ImGui::MenuItem("Exit")) { exit(0); }
					ImGui::EndMenu();
				}
#ifdef _DEBUG
				if (ImGui::BeginMenu("DBG"))
				{
					ImGui::MenuItem("ImGui Demo", NULL, &showguidemo);
					ImGui::MenuItem("ImPlot Demo", NULL, &showplotdemo);
					ImGui::EndMenu();
				}
#endif // _DEBUG
				ImGui::EndMenuBar();
			}

			ImGui::End();
		}

		{
			if (open)
				ImGui::OpenPopup("Open File");
			if (file_dialog.showFileDialog(&open, "Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), "*.*"))
			{
                if (file.is_open()) {
                    file.close();
                }
				path_to_file = file_dialog.selected_path;
                fileIsAnalyzed = false;
			}
		}

        if (!fileIsAnalyzed && !path_to_file.empty())
        {
            file = std::ifstream(path_to_file, std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "Failed to open file: " << path_to_file << std::endl;
                exit(1);
            }
            systemKProcessAddress = findSystemKProcessAddress(file);

            if (systemKProcessAddress == 0)
            {
                std::cerr << "Failed to find systemKProcessAddress" << std::endl;
                exit(1);
            }
            processList = getProcessList(systemKProcessAddress, _CR3, file);
            fileIsAnalyzed = true;
        }

		if (fileIsAnalyzed)
		{
			bool windowopened = true;
			ImGui::Begin("Dump analyzer", &windowopened);
			ImGui::Text("Current file: %s", path_to_file.c_str());
			ImGui::Separator();

            static int item_current_idx = 0;
            const char* combo_preview_value = processList[item_current_idx].ProcessName.c_str();

            if (ImGui::BeginCombo("Process", combo_preview_value))
            {
                for (int n = 0; n < processList.size(); n++)
                {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(processList[n].ProcessName.c_str(), is_selected)) {
                        item_current_idx = n;
                    }

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

			ImGui::Separator();
			ImGui::Text("VAD nodes:");
			if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_Borders))
			{
				ImGui::TableSetupColumn("StartAddress", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("EndAddress", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableHeadersRow();

                for (auto & processVadNode : processList[item_current_idx].VadTree) {
                    ImGui::TableNextRow();
                    for (int column = 0; column < 2; column++)
                    {
                        ImGui::TableSetColumnIndex(column);
                        if (column == 0) {
                            ImGui::Text("%llx", processVadNode.startAddress);
                        }
                        else {
                            ImGui::Text("%llx", processVadNode.endAddress);
                        }
                    }
                }

				ImGui::EndTable();
			}

			ImGui::End();
			if (!windowopened)
				path_to_file.clear();
		}




#ifdef _DEBUG
			if (showguidemo)
			{
				ImGui::ShowDemoWindow();
			}
			if (showplotdemo)
			{
				ImPlot::ShowDemoWindow();
			}
#endif // DEBUG

		


		
		
		gui.Render();
	}
}
