namespace CompressionTest
{
    partial class CompressionTest
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.SplitContainer splitContainer1;
            System.Windows.Forms.Panel panel1;
            System.Windows.Forms.Button StreamFilesButton;
            System.Windows.Forms.Button CompressFilesButton;
            System.Windows.Forms.Button RemoveFilesButton;
            System.Windows.Forms.Button SelectFilesButton;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CompressionTest));
            this.mFileList = new System.Windows.Forms.ListView();
            this.OutputTextBox = new System.Windows.Forms.TextBox();
            this.OFD = new System.Windows.Forms.OpenFileDialog();
            this.ToolTip = new System.Windows.Forms.ToolTip(this.components);
            this.AlgorithmCombo = new System.Windows.Forms.ComboBox();
            splitContainer1 = new System.Windows.Forms.SplitContainer();
            panel1 = new System.Windows.Forms.Panel();
            StreamFilesButton = new System.Windows.Forms.Button();
            CompressFilesButton = new System.Windows.Forms.Button();
            RemoveFilesButton = new System.Windows.Forms.Button();
            SelectFilesButton = new System.Windows.Forms.Button();
            splitContainer1.Panel1.SuspendLayout();
            splitContainer1.Panel2.SuspendLayout();
            splitContainer1.SuspendLayout();
            panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            splitContainer1.Location = new System.Drawing.Point(0, 0);
            splitContainer1.Name = "splitContainer1";
            splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            splitContainer1.Panel1.Controls.Add(this.mFileList);
            splitContainer1.Panel1.Controls.Add(panel1);
            // 
            // splitContainer1.Panel2
            // 
            splitContainer1.Panel2.Controls.Add(this.OutputTextBox);
            splitContainer1.Size = new System.Drawing.Size(787, 475);
            splitContainer1.SplitterDistance = 208;
            splitContainer1.TabIndex = 0;
            // 
            // mFileList
            // 
            this.mFileList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mFileList.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.mFileList.Location = new System.Drawing.Point(40, 0);
            this.mFileList.Name = "mFileList";
            this.mFileList.Size = new System.Drawing.Size(747, 208);
            this.mFileList.TabIndex = 1;
            this.mFileList.UseCompatibleStateImageBehavior = false;
            this.mFileList.View = System.Windows.Forms.View.Details;
            // 
            // panel1
            // 
            panel1.Controls.Add(this.AlgorithmCombo);
            panel1.Controls.Add(StreamFilesButton);
            panel1.Controls.Add(CompressFilesButton);
            panel1.Controls.Add(RemoveFilesButton);
            panel1.Controls.Add(SelectFilesButton);
            panel1.Dock = System.Windows.Forms.DockStyle.Left;
            panel1.Location = new System.Drawing.Point(0, 0);
            panel1.Name = "panel1";
            panel1.Size = new System.Drawing.Size(40, 208);
            panel1.TabIndex = 0;
            // 
            // StreamFilesButton
            // 
            StreamFilesButton.Image = global::CompressionTest.Properties.Resources.Stream;
            StreamFilesButton.Location = new System.Drawing.Point(5, 138);
            StreamFilesButton.Name = "StreamFilesButton";
            StreamFilesButton.Size = new System.Drawing.Size(32, 32);
            StreamFilesButton.TabIndex = 3;
            this.ToolTip.SetToolTip(StreamFilesButton, "Uncompress files");
            StreamFilesButton.UseVisualStyleBackColor = true;
            StreamFilesButton.Click += new System.EventHandler(this.StreamFilesButton_Click);
            // 
            // CompressFilesButton
            // 
            CompressFilesButton.Image = global::CompressionTest.Properties.Resources.Compress;
            CompressFilesButton.Location = new System.Drawing.Point(5, 100);
            CompressFilesButton.Name = "CompressFilesButton";
            CompressFilesButton.Size = new System.Drawing.Size(32, 32);
            CompressFilesButton.TabIndex = 2;
            this.ToolTip.SetToolTip(CompressFilesButton, "Compress files");
            CompressFilesButton.UseVisualStyleBackColor = true;
            CompressFilesButton.Click += new System.EventHandler(this.CompressFilesButton_Click);
            // 
            // RemoveFilesButton
            // 
            RemoveFilesButton.Image = global::CompressionTest.Properties.Resources.RemoveFiles;
            RemoveFilesButton.Location = new System.Drawing.Point(5, 35);
            RemoveFilesButton.Name = "RemoveFilesButton";
            RemoveFilesButton.Size = new System.Drawing.Size(32, 32);
            RemoveFilesButton.TabIndex = 1;
            this.ToolTip.SetToolTip(RemoveFilesButton, "Remove selected files");
            RemoveFilesButton.UseVisualStyleBackColor = true;
            RemoveFilesButton.Click += new System.EventHandler(this.RemoveFilesButton_Click);
            // 
            // SelectFilesButton
            // 
            SelectFilesButton.Image = global::CompressionTest.Properties.Resources.AddFiles;
            SelectFilesButton.Location = new System.Drawing.Point(5, 3);
            SelectFilesButton.Name = "SelectFilesButton";
            SelectFilesButton.Size = new System.Drawing.Size(32, 32);
            SelectFilesButton.TabIndex = 0;
            this.ToolTip.SetToolTip(SelectFilesButton, "Add files");
            SelectFilesButton.UseVisualStyleBackColor = true;
            SelectFilesButton.Click += new System.EventHandler(this.SelectFilesButton_Click);
            // 
            // OutputTextBox
            // 
            this.OutputTextBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.OutputTextBox.Location = new System.Drawing.Point(0, 0);
            this.OutputTextBox.Multiline = true;
            this.OutputTextBox.Name = "OutputTextBox";
            this.OutputTextBox.ReadOnly = true;
            this.OutputTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.OutputTextBox.Size = new System.Drawing.Size(787, 263);
            this.OutputTextBox.TabIndex = 0;
            // 
            // OFD
            // 
            this.OFD.AddExtension = false;
            this.OFD.Multiselect = true;
            this.OFD.Title = "Select Files";
            // 
            // AlgorithmCombo
            // 
            this.AlgorithmCombo.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.AlgorithmCombo.DropDownWidth = 100;
            this.AlgorithmCombo.FormattingEnabled = true;
            this.AlgorithmCombo.Location = new System.Drawing.Point(5, 73);
            this.AlgorithmCombo.Name = "AlgorithmCombo";
            this.AlgorithmCombo.Size = new System.Drawing.Size(32, 21);
            this.AlgorithmCombo.TabIndex = 4;
            this.ToolTip.SetToolTip(this.AlgorithmCombo, "Compression algorithm to use (decompression algorithm is selected automatically)");
            // 
            // CompressionTest
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(787, 475);
            this.Controls.Add(splitContainer1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "CompressionTest";
            this.Text = "Compression and Streaming Tester";
            splitContainer1.Panel1.ResumeLayout(false);
            splitContainer1.Panel2.ResumeLayout(false);
            splitContainer1.Panel2.PerformLayout();
            splitContainer1.ResumeLayout(false);
            panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog OFD;
        private System.Windows.Forms.ListView mFileList;
        private System.Windows.Forms.ToolTip ToolTip;
        private System.Windows.Forms.TextBox OutputTextBox;
        private System.Windows.Forms.ComboBox AlgorithmCombo;
    }
}

