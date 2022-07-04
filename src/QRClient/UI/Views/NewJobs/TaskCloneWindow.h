#pragma once

#include <QWidget>
#include "Dialog.h"
#include "ui_TaskCloneWindow.h"

class TaskCloneWindow : public Dialog
{
    Q_OBJECT

public:
    TaskCloneWindow(t_taskid taskId, QWidget *parent = Q_NULLPTR);
    ~TaskCloneWindow();
    void initUi(bool isWindow = true);
	void initUi(const double& lon, const double& lat,bool isWindow = true);
    bool getRebuildParams(RebuildOptionParams &param);
    QWidget* getParamWidget();
    void resetView() { initUi(false); }
	void resetView(const double &lon, const double &lat) { initUi(lon,lat,false); }

private slots:
    void on_btnOk_clicked();
    void on_checkBox_lockRange_stateChanged(int state);
    void on_checkBox_xyz_offset_cloneWindow_stateChanged(int state);
	void on_checkBox_enable_custom_stateChanged(int state);
    void on_btnRangeFile_clicked();
    void on_comboBox_tile_mode_currentIndexChanged(int);

private:
    void SetErrorMessage(const QString &message);

	bool checkIllegalpath(const QString& path);

private:
    Ui::TaskCloneWindow ui;
    t_taskid m_cloneTaskId;
    RebuildOptionParams m_rebuildParams;
    bool m_isWindow;
};
