using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using CompressionInterop;


namespace CompressionTest
{
    public partial class CompressionTest : Form
    {
        private List<String> mOutputLines = new List<String>();

        public CompressionTest()
        {
            InitializeComponent();

            ColumnHeader ch = new ColumnHeader();
            ch.Name = "Files";
            ch.Text = "Files";
            ch.AutoResize(ColumnHeaderAutoResizeStyle.HeaderSize);
            mFileList.Columns.Add(ch);

            String[] algorithms = Enum.GetNames(typeof(CompressionInterop.Algorithm));
            foreach (String algorithm in algorithms)
            {
                AlgorithmCombo.Items.Add(algorithm);
            }
            AlgorithmCombo.SelectedIndex = 0;
        }

        private void SelectFilesButton_Click(object sender, EventArgs e)
        {
            if (OFD.ShowDialog(this) == DialogResult.OK)
            {
                mFileList.BeginUpdate();

                foreach (String file in OFD.FileNames)
                {
                    ListViewItem[] found = mFileList.Items.Find(file, false);
                    if (found.Length == 0)
                    {
                        ListViewItem lvi = mFileList.Items.Add(file);
                        lvi.Name = file;
                    }
                }

                mFileList.Columns[0].AutoResize(ColumnHeaderAutoResizeStyle.ColumnContent);
                mFileList.EndUpdate();
            }
        }

        private void RemoveFilesButton_Click(object sender, EventArgs e)
        {
            mFileList.BeginUpdate();

            foreach (ListViewItem lvi in mFileList.SelectedItems)
            {
                mFileList.Items.Remove(lvi);
            }

            mFileList.Columns[0].AutoResize(ColumnHeaderAutoResizeStyle.ColumnContent);
            mFileList.EndUpdate();
        }

        private void OutputLine(String line)
        {
            mOutputLines.Add(line);
            OutputTextBox.Lines = mOutputLines.ToArray();
            OutputTextBox.Refresh();
        }

        private void CompressFilesButton_Click(object sender, EventArgs e)
        {
            String selectedAlgorithm = AlgorithmCombo.SelectedItem as String;
            CompressionInterop.Algorithm algorithm = (CompressionInterop.Algorithm)Enum.Parse(typeof(CompressionInterop.Algorithm), selectedAlgorithm);

            OutputLine("Compressing with " + selectedAlgorithm + "...");

            int totalUncompressedDataSize = 0;
            int totalCompressedDataSize = 0;
            int numFilesCompressed = 0;
            double totalElapsedTime = 0.0f;

            CompressionInterop.Suite compressor = new CompressionInterop.Suite();
            CompressionInterop.Stats stats = new CompressionInterop.Stats();
            foreach (ListViewItem lvi in mFileList.Items)
            {
                String inputFileName = lvi.Text;
                String ext = Path.GetExtension(inputFileName);
                String outputFileName = getOutputFileName(inputFileName, "Compressed", ext + "-ctd");

                bool result = compressor.Compress(inputFileName, algorithm, outputFileName, ref stats);

                if (result)
                {
                    totalUncompressedDataSize += stats.UncompressedDataSize;
                    totalCompressedDataSize += stats.CompressedDataSize;
                    totalElapsedTime += stats.ElapsedTime;
                    numFilesCompressed++;

                    float ratio = ((float)stats.CompressedDataSize / (float)stats.UncompressedDataSize) * 100.0f;
                    StringBuilder sb = new StringBuilder(256);
                    sb.AppendFormat("Compressed {0}. Compression ratio: {1:0.000}% ({2}b -> {3}b). Time: {4:0.00000}s", inputFileName, ratio, stats.UncompressedDataSize, stats.CompressedDataSize, stats.ElapsedTime);
                    OutputLine(sb.ToString());
                }
                else
                {
                    StringBuilder sb = new StringBuilder(256);
                    sb.AppendFormat("Error compressing {0}!", inputFileName);
                    OutputLine(sb.ToString());
                }
            }

            if (numFilesCompressed > 0)
            {
                float ratio = ((float)totalCompressedDataSize / (float)totalUncompressedDataSize) * 100.0f;
                StringBuilder sb = new StringBuilder(256);
                sb.AppendFormat("Total: Compressed {0} files. Compression ratio: {1:0.000}% ({2}b -> {3}b). Time: {4:0.00000}s", numFilesCompressed, ratio, totalUncompressedDataSize, totalCompressedDataSize, totalElapsedTime);
                OutputLine(sb.ToString());
            }
        }

        private static String getOutputFileName(String inputFileName, String subDir, String ext)
        {
            String dir = Path.GetDirectoryName(inputFileName);

            String outDir = Path.Combine(dir, subDir);
            if (!Directory.Exists(outDir))
                Directory.CreateDirectory(outDir);

            String file = Path.GetFileName(Path.ChangeExtension(inputFileName, ext));

            String outputFileName = Path.Combine(outDir, file);
            return outputFileName;
        }

        private void StreamFilesButton_Click(object sender, EventArgs e)
        {
            OutputLine("Uncompressing...");

            int totalUncompressedDataSize = 0;
            int totalCompressedDataSize = 0;
            int numFilesUncompressed = 0;
            double totalElapsedTime = 0.0f;

            CompressionInterop.Suite decompressor = new CompressionInterop.Suite();
            CompressionInterop.Stats stats = new CompressionInterop.Stats();

            foreach (ListViewItem lvi in mFileList.Items)
            {
                String ext = Path.GetExtension(lvi.Text);
                String inputFileName = getOutputFileName(lvi.Text, "Compressed", ext + "-ctd");
                String outputFileName = getOutputFileName(lvi.Text, "Uncompressed", ext);
                if (!File.Exists(inputFileName))
                    continue;

                Byte[] uncompressedData;
                bool result = decompressor.Decompress(inputFileName, out uncompressedData, ref stats);

                if (result)
                {
                    totalUncompressedDataSize += stats.UncompressedDataSize;
                    totalCompressedDataSize += stats.CompressedDataSize;
                    totalElapsedTime += stats.ElapsedTime;
                    numFilesUncompressed++;

                    using (FileStream stream = new FileStream(outputFileName, FileMode.Create))
                    {
                        using (BinaryWriter writer = new BinaryWriter(stream))
                        {
                            writer.Write(uncompressedData);
                            writer.Close();
                        }
                    }

                    StringBuilder sb = new StringBuilder(256);
                    sb.AppendFormat("Uncompressed {0} from {1}b -> {2}b in {3:0.00000}s", inputFileName, stats.CompressedDataSize, stats.UncompressedDataSize, stats.ElapsedTime);
                    OutputLine(sb.ToString());
                }
                else
                {
                    StringBuilder sb = new StringBuilder(256);
                    sb.AppendFormat("Error uncompressing {0}!", inputFileName);
                    OutputLine(sb.ToString());
                }
            }

            if (numFilesUncompressed > 0)
            {
                StringBuilder sb = new StringBuilder(256);
                sb.AppendFormat("Total: Uncompressed {0} files ({1}b -> {2}b). Time: {3:0.00000}s", numFilesUncompressed, totalCompressedDataSize, totalUncompressedDataSize, totalElapsedTime);
                OutputLine(sb.ToString());
            }
        }
    }
}
