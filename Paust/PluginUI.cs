using System;
using System.Linq.Expressions;
using System.Numerics;
using System.Reflection;
using Dalamud.Interface;
using Dalamud.Interface.Windowing;
using Dalamud.Logging;
using ImGuiNET;

namespace Paust
{
    internal class PluginUI : Window, IDisposable
    {
        private readonly Plugin plugin;

        private readonly ImFontPtr font;

        public unsafe PluginUI(Plugin plugin) : base("PaustConfig")
        {
            this.plugin = plugin;

            this.Size = new Vector2(-1, -1);
            this.SizeCondition = ImGuiCond.Appearing;
            
            font = ImGui.GetIO().Fonts.AddFontFromFileTTF("D2Coding-Ver1.3.2-20180524.ttf", 16.0f, ImGui.GetIO().Fonts.GetGlyphRangesKorean());
        }
        ~PluginUI()
        {
            this.Dispose(false);
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }
        private bool disposed;
        protected void Dispose(bool disposing)
        {
            if (this.disposed) return;
            this.disposed = true;

            if (disposing)
            {
                this.font.Destroy();
            }
        }

        private string errorMessage;
        private string presetName;
        private string presetNameOld;
        private string presetJavascript;
        private float editPopupButton = 100;

        private float[] presetButtonWidth = new float[3];

        public override void Draw()
        {
            lock (this.plugin.Config)
            {
                var changed = true;

                changed |= this.AddCheckbox("긴 닉네임 줄이기##short-name", e => e.ShortName);
                changed |= this.AddCheckbox("타 서버 표시 지우기##hide-server", e => e.HideServer);
                changed |= this.AddCheckbox("임무 정보 표시 끄기##hide-option", e => e.HideOptions);
                changed |= this.AddCheckbox("확직 자리를 참가중으로 표시##modify-reservation", e => e.ModifyReservation);

                ImGui.Spacing();
                ImGui.Separator();
                ImGui.Spacing();

                ////////////////////////////////////////////////////////////////////////////////////////////////////

                PluginConfig.Preset preset;

                if (this.plugin.Config.SelectedPreset != Guid.Empty)
                {
                    if (!this.plugin.Config.Presets.TryGetValue(this.plugin.Config.SelectedPreset, out preset))
                    {
                        preset = PluginConfig.Preset.Empty;
                        changed = true;
                    }
                }
                else
                {
                    preset = PluginConfig.Preset.Empty;
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////

                var spacing = ImGui.GetStyle().ItemSpacing;

                ImGui.TextUnformatted("Preset");

                ImGui.SetNextItemWidth(-this.presetButtonWidth[0] - this.presetButtonWidth[1] - this.presetButtonWidth[2] - spacing.X * 3);
                if (ImGui.BeginCombo("###preset", preset.Name))
                {
                    if (ImGui.Selectable(PluginConfig.Preset.Empty.NameId))
                    {
                        preset = PluginConfig.Preset.Empty;
                        this.plugin.Config.SelectedPreset = preset.Guid;
                        changed = true;
                    }

                    foreach (var p in this.plugin.Config.Presets)
                    {
                        if (ImGui.Selectable(p.Value.NameId))
                        {
                            preset = p.Value;
                            this.plugin.Config.SelectedPreset = preset.Guid;
                            changed = true;
                        }
                    }

                    ImGui.EndCombo();
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////

                ImGui.SameLine();
                if (IconButton(FontAwesomeIcon.Plus, "preset-add", true))
                {
                    preset = new PluginConfig.Preset
                    {
                        Guid = Guid.NewGuid(),
                        Name = "새 프리셋",
                        Javascript = "",
                    };

                    this.plugin.Config.SelectedPreset = preset.Guid;
                    this.plugin.Config.Presets.Add(preset.Guid, preset);

                    changed = true;
                }
                this.presetButtonWidth[0] = ImGui.GetItemRectSize().X;

                ////////////////////////////////////////////////////////////////////////////////////////////////////

                ImGui.SameLine();
                if (IconButton(FontAwesomeIcon.PencilAlt, "preset-edit", !preset.IsEmpty))
                {
                    this.presetName = preset.Name;
                    this.presetNameOld = preset.Name;
                    this.presetJavascript = preset.Javascript;
                    ImGui.OpenPopup("###preset-edit-popup");
                }
                this.presetButtonWidth[1] = ImGui.GetItemRectSize().X;

                if (ImGui.BeginPopupModal("프리셋 편집###preset-edit-popup") && !preset.IsEmpty)
                {
                    ImGui.PushItemWidth(-1f);

                    var presetNameNew = this.presetName;
                    ImGui.TextUnformatted("프리셋 이름:");
                    if (ImGui.InputText("##preset-name", ref presetNameNew, 128))
                    {
                        this.presetName = presetNameNew.Trim();
                    }

                    var presetJavascriptNew = this.presetJavascript;
                    ImGui.TextUnformatted("프리셋 스크립트:");
                    ImGui.PushFont(this.font);
                    if (ImGui.InputTextMultiline("##preset-javascript", ref presetJavascriptNew, 128 * 1024, new Vector2(-1, -this.editPopupButton - spacing.Y)))
                    {
                        this.presetJavascript = presetJavascriptNew.Trim();
                    }
                    ImGui.PopFont();

                    ImGui.PopItemWidth();

                    if (ImGui.Button("저장"))
                    {
                        var passed = true;

                        var presetNameChanged = true;
                        var presetJavascriptChanged = true;

                        if (string.IsNullOrWhiteSpace(this.presetName))
                        {
                            passed = false;
                            this.errorMessage = "프리셋 이름은 비워둘 수 없습니다.";
                            this.presetName = this.presetNameOld;
                        }
                        else
                        {
                            PluginLog.Debug("new presetName : {0}", this.presetName);
                            presetNameChanged = true;
                        }

                        if (preset.Javascript != this.presetJavascript)
                        {
                            PluginLog.Debug("new presetJavascript : {0}", this.presetJavascript);

                            if (string.IsNullOrWhiteSpace(this.presetJavascript))
                            {
                                presetJavascriptChanged = true;
                            }
                            else
                            {
                                try
                                {
                                    this.plugin.JintWrapper.Eval(this.presetJavascript, JintWrapper.SampleListing);

                                    presetJavascriptChanged = true;
                                }
                                catch (Exception ex)
                                {
                                    PluginLog.Debug(ex, "Paust Javascript Exception");
                                    this.errorMessage = ex.Message;
                                    passed = false;
                                }
                            }
                        }

                        if (passed)
                        {
                            if (presetNameChanged      ) preset.Name       = this.presetName;
                            if (presetJavascriptChanged) preset.Javascript = this.presetJavascript;

                            changed = true;

                            ImGui.CloseCurrentPopup();
                        }
                    }

                    ImGui.SameLine();
                    if (ImGui.Button("닫기"))
                    {
                        ImGui.CloseCurrentPopup();
                    }
                    this.editPopupButton = ImGui.GetItemRectSize().Y;

                    if (errorMessage != null)
                    {
                        ImGui.OpenPopup("###preset-javascript-error");
                    }
                    if (ImGui.BeginPopupModal("오류 발생###preset-javascript-error"))
                    {
                        ImGui.TextUnformatted(errorMessage);

                        ImGui.Separator();

                        if (ImGui.Button("닫기"))
                        {
                            this.errorMessage = null;
                            ImGui.CloseCurrentPopup();
                        }

                        ImGui.EndPopup();
                    }

                    ImGui.EndPopup();
                }

                ////////////////////////////////////////////////////////////////////////////////////////////////////

                ImGui.SameLine();
                if (IconButton(FontAwesomeIcon.Trash, "preset-delete", !preset.IsEmpty))
                {
                    _ = this.plugin.Config.Presets.Remove(preset.Guid);
                    preset = PluginConfig.Preset.Empty;

                    changed = true;
                }
                this.presetButtonWidth[2] = ImGui.GetItemRectSize().X;

                ////////////////////////////////////////////////////////////////////////////////////////////////////

                if (changed)
                {
                    this.plugin.Config.Save();
                }
            }
        }

        private static bool IconButton(FontAwesomeIcon icon, string id, bool enabled)
        {
            var text = FontAwesomeExtensions.ToIconString(icon);
            if (!string.IsNullOrWhiteSpace(id))
            {
                text = $"{text}##{id}";
            }

            ImGui.PushFont(UiBuilder.IconFont);
            if (!enabled)
            {
                ImGui.PushStyleVar(ImGuiStyleVar.Alpha, 0.2f);
            }
            var result = ImGui.Button(text);
            if (!enabled)
            {
                ImGui.PopStyleVar();
            }
            ImGui.PopFont();
            return result && enabled;
        }

        private bool AddCheckbox(string name, Expression<Func<PluginConfig, bool>> expression)
        {
            var memberSelectorExpression = expression.Body as MemberExpression;
            var property = memberSelectorExpression.Member as PropertyInfo;

            var valueOld = (bool)property.GetValue(this.plugin.Config);
            var valueNew = valueOld;
            if (ImGui.Checkbox(name, ref valueNew) && valueOld != valueNew)
            {
                PluginLog.Debug("{0} : {1} -> {2}", name, valueOld, valueNew);

                property.SetValue(this.plugin.Config, valueNew);
                return true;
            }

            return false;
        }
    }
}
