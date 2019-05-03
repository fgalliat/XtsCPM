import java.awt.BorderLayout;
import java.awt.EventQueue;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JButton;
import javax.swing.JEditorPane;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.filechooser.FileSystemView;

import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import javax.swing.JComboBox;

public class TerminalFrame {

	protected List<String> history = new ArrayList<String>();

	private JEditorPane editorPane;
	private JFrame frame;
	private JTextField textField;
	private JTextField textField_PRG;
	private JScrollPane scrollPane;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) throws Exception {
//		UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");
		UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");

		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					TerminalFrame window = new TerminalFrame();
					window.frame.setVisible(true);

					window.connect();

				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	// ======================================
	private boolean isViewAtBottom() {
		JScrollBar sb = getScrollPane().getVerticalScrollBar();
		int min = sb.getValue() + sb.getVisibleAmount();
		int max = sb.getMaximum();
		System.out.println(min + " " + max);
		return min == max;
	}

	private void scrollToBottom() {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				getScrollPane().getVerticalScrollBar().setValue(getScrollPane().getVerticalScrollBar().getMaximum());
			}
		});
	}

	public void console(Object o) {
		editorPane.setText(editorPane.getText() + "\n" + o);

//		if ( !isViewAtBottom() ) {
		scrollToBottom();
//		}
//			setCaretPosition(Component.getDocument().getLength());

	}

	public void connect() {
		boolean ok = Terminal.getInstance().reconnect(true);
		if (ok) {
			Terminal.getInstance().addPortListener(new PortReader());
			console("Connected !");
		} else {
			console("Not Connected !");
		}
	}

	public void reboot() {
		boolean ok = Terminal.getInstance().reboot();
	}

	public void serial() {
		boolean ok = Terminal.getInstance().serial();
	}

	// PRGM
	public void cat() {
		boolean ok = Terminal.getInstance().cat();
	}

	public void editPRG() {
		String prgm = getPRGname();
		if (prgm == null) {
			return;
		}
		try {
			Terminal.getInstance().sendCommand("editprg(\"" + prgm + "\");");
		} catch (Exception e) {
			e.printStackTrace();
		}
		return;
	}

	public void listPRG() {
		String prgm = getPRGname();
		if (prgm == null) {
			return;
		}
		try {
			Terminal.getInstance().sendCommand("listprg(\"" + prgm + "\");");
		} catch (Exception e) {
			e.printStackTrace();
		}
		return;
	}

	public void runPRG() {
		String prgm = getPRGname();
		if (prgm == null) {
			return;
		}
		try {
			Terminal.getInstance().sendCommand("runprg(\"" + prgm + "\");");
		} catch (Exception e) {
			e.printStackTrace();
		}
		return;
	}

	protected class PortReader implements SerialPortEventListener {

		@Override
		public void serialEvent(SerialPortEvent event) {

			if (Terminal.getInstance().isListenerLocked()) {
				return;
			}

			if (event.isRXCHAR() && event.getEventValue() > 0) {
				try {
					String receivedData = Terminal.getInstance().readString(event.getEventValue());
//					System.out.println("Received response: " + receivedData);
					console(receivedData);
				} catch (SerialPortException ex) {
					System.out.println("Error in receiving string from COM-port: " + ex);
				}
			}
		}

	}

	protected boolean sendFile(boolean autorun) {
		// JFileChooser jfc = new JFileChooser("C:\\vm_mnt\\sync_box\\devl\\Arduino\\Teensy3.6\\bitlashTest\\data\\"
		JFileChooser jfc = new JFileChooser("C:\\vm_mnt\\sync_box\\devl\\Arduino\\Teensy3.6\\XtsBitlash\\data\\"
//				FileSystemView.getFileSystemView().getHomeDirectory()
		);

		int returnValue = jfc.showOpenDialog(null);
		// int returnValue = jfc.showSaveDialog(null);

		if (returnValue == JFileChooser.APPROVE_OPTION) {
			File selectedFile = jfc.getSelectedFile();
			System.out.println(selectedFile.getAbsolutePath());

			try { Terminal.getInstance().takeHand(); }
			catch(Exception ex) {
				console("(!!) Failed to take hand");
				console(ex.toString());
			}
			
			Terminal.getInstance().serial();
			
			Terminal.getInstance().sendFile(selectedFile.getAbsolutePath(), selectedFile.getName(), true);
			Zzz(1500);
//			try {
//				String resp = Terminal.getInstance().readSerLine(1000, true);
//				System.err.println(resp);
//			} catch (Exception ex) {
//			}

			if (autorun) {
				String fname = selectedFile.getName();
				String ext = fname;
				ext = ext.substring(ext.lastIndexOf('.')).toLowerCase();
				try {
					if (ext.equals(".t53") || ext.equals(".t5k")) {
						Terminal.getInstance().sendCommand("play(\"" + fname + "\"):");
					} else if (ext.equals(".bmp")) {
						Terminal.getInstance().sendCommand("bmp(\"" + fname + "\"):");
					} else if (ext.equals(".prg")) {
						Terminal.getInstance()
								.sendCommand("runprg(\"" + fname.substring(0, fname.length() - 4) + "\"):");
					}
				} catch (Exception ex) {
					ex.printStackTrace();
					return false;
				}
				try { // consume ECHO
					String resp = Terminal.getInstance().readSerLine(1000, true);
					System.err.println(resp);
				} catch (Exception ex) {
				}
			}

			return true;
		}
		return false;
	}

	protected void dir(String ext, JComboBox combo) {
		// List<String> content = Terminal.getInstance().sendListCmd("dir(\""+ ext +"\");");
		combo.addItem("BMP1...");
		combo.addItem("BMP2...");
		combo.addItem("BMP3...");
	};
	
	// ======================================

	private void Zzz(int i) {
		try {
			Thread.sleep(i);
		} catch (Exception e) {
		}
	}

	/**
	 * Create the application.
	 */
	public TerminalFrame() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame("Yatl on port " + Terminal.getInstance().getCommPort());
		frame.setBounds(100, 100, 1024, 600);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		JPanel panel = new JPanel();
		frame.getContentPane().add(panel, BorderLayout.NORTH);
		GridBagLayout gbl_panel = new GridBagLayout();
		gbl_panel.columnWidths = new int[] {0, 0, 0, 0, 0, 0, 0, 0, 30, 30, 0, 0, 0, 0, 0};
		gbl_panel.rowHeights = new int[] {0};
		gbl_panel.columnWeights = new double[] { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, Double.MIN_VALUE };
		gbl_panel.rowWeights = new double[] { 0.0 };
		panel.setLayout(gbl_panel);

		JButton btnConn = new JButton("CONN.");
		btnConn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				connect();
			}
		});
		GridBagConstraints gbc_btnConn = new GridBagConstraints();
		gbc_btnConn.insets = new Insets(0, 0, 0, 5);
		gbc_btnConn.gridx = 0;
		gbc_btnConn.gridy = 0;
		panel.add(btnConn, gbc_btnConn);

		JButton btnReset = new JButton("RST");
		btnReset.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				reboot();
			}
		});
		GridBagConstraints gbc_btnReset = new GridBagConstraints();
		gbc_btnReset.insets = new Insets(0, 0, 0, 5);
		gbc_btnReset.gridx = 1;
		gbc_btnReset.gridy = 0;
		panel.add(btnReset, gbc_btnReset);

		JButton btnLoad = new JButton("LOAD");
		btnLoad.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
			}
		});

		JButton btnSerialIo = new JButton("Console");
		btnSerialIo.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				serial();
			}
		});
		
		JButton btnNewButton_1 = new JButton("<-");
		btnNewButton_1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				try {
					Terminal.getInstance().serWrite(5);
				} catch(Exception ex) {
					System.err.println("Failed to strike key");
				}
			}
		});
		GridBagConstraints gbc_btnNewButton_1 = new GridBagConstraints();
		gbc_btnNewButton_1.insets = new Insets(0, 0, 0, 5);
		gbc_btnNewButton_1.gridx = 2;
		gbc_btnNewButton_1.gridy = 0;
		panel.add(btnNewButton_1, gbc_btnNewButton_1);
		
		JButton button_1 = new JButton("->");
		button_1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				try {
					Terminal.getInstance().serWrite(6);
				} catch(Exception ex) {
					System.err.println("Failed to strike key");
				}
			}
		});
		GridBagConstraints gbc_button_1 = new GridBagConstraints();
		gbc_button_1.insets = new Insets(0, 0, 0, 5);
		gbc_button_1.gridx = 3;
		gbc_button_1.gridy = 0;
		panel.add(button_1, gbc_button_1);
		
		JButton btnn = new JButton("\\r");
		btnn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				try {
					Terminal.getInstance().serWrite(13);
				} catch(Exception ex) {
					System.err.println("Failed to strike key");
				}
			}
		});
		GridBagConstraints gbc_btnn = new GridBagConstraints();
		gbc_btnn.insets = new Insets(0, 0, 0, 5);
		gbc_btnn.gridx = 4;
		gbc_btnn.gridy = 0;
		panel.add(btnn, gbc_btnn);
		GridBagConstraints gbc_btnSerialIo = new GridBagConstraints();
		gbc_btnSerialIo.insets = new Insets(0, 0, 0, 5);
		gbc_btnSerialIo.gridx = 5;
		gbc_btnSerialIo.gridy = 0;
		panel.add(btnSerialIo, gbc_btnSerialIo);
		GridBagConstraints gbc_btnLoad = new GridBagConstraints();
		gbc_btnLoad.insets = new Insets(0, 0, 0, 5);
		gbc_btnLoad.gridx = 6;
		gbc_btnLoad.gridy = 0;
		panel.add(btnLoad, gbc_btnLoad);

		JButton btnSave = new JButton("SAVE");
		btnSave.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
			}
		});
		GridBagConstraints gbc_btnSave = new GridBagConstraints();
		gbc_btnSave.insets = new Insets(0, 0, 0, 5);
		gbc_btnSave.gridx = 7;
		gbc_btnSave.gridy = 0;
		panel.add(btnSave, gbc_btnSave);
				
						JButton btnSend = new JButton("SEND");
						btnSend.addActionListener(new ActionListener() {
							public void actionPerformed(ActionEvent e) {
								// EventQueue.invokeLater
								new Thread(new Runnable() {
									public void run() {
										sendFile(false);
									}
								}).start();
							}
						});
						GridBagConstraints gbc_btnSend = new GridBagConstraints();
						gbc_btnSend.insets = new Insets(0, 0, 0, 5);
						gbc_btnSend.gridx = 8;
						gbc_btnSend.gridy = 0;
						panel.add(btnSend, gbc_btnSend);
		
				JButton btnSndrun = new JButton("SND+RUN");
				btnSndrun.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						new Thread(new Runnable() {
							public void run() {
								sendFile(true);
							}
						}).start();
					}
				});
				GridBagConstraints gbc_btnSndrun = new GridBagConstraints();
				gbc_btnSndrun.insets = new Insets(0, 0, 0, 5);
				gbc_btnSndrun.gridx = 9;
				gbc_btnSndrun.gridy = 0;
				panel.add(btnSndrun, gbc_btnSndrun);
		
		JComboBox comboBMP = new JComboBox();
		GridBagConstraints gbc_comboBMP = new GridBagConstraints();
		gbc_comboBMP.insets = new Insets(0, 0, 0, 5);
		gbc_comboBMP.fill = GridBagConstraints.HORIZONTAL;
		gbc_comboBMP.gridx = 10;
		gbc_comboBMP.gridy = 0;
		panel.add(comboBMP, gbc_comboBMP);
		
		JButton button = new JButton("R");
		button.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				new Thread() {
					public void run() {
						comboBMP.addItem("Please select a BMP...");
						 dir(".bmp", comboBMP);
					}

				}.start();
			}
		});
		GridBagConstraints gbc_button = new GridBagConstraints();
		gbc_button.insets = new Insets(0, 0, 0, 5);
		gbc_button.gridx = 11;
		gbc_button.gridy = 0;
		panel.add(button, gbc_button);

		editorPane = new JEditorPane();
		scrollPane = new JScrollPane(editorPane);
		frame.getContentPane().add(scrollPane, BorderLayout.CENTER);

		JPanel panel_1 = new JPanel();
		frame.getContentPane().add(panel_1, BorderLayout.WEST);
		panel_1.setLayout(new GridLayout(0, 1, 0, 0));

		JButton btnCat = new JButton("CAT");
		btnCat.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				cat();
			}
		});
		panel_1.add(btnCat);

		JButton btnRun = new JButton("RUN");
		btnRun.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				runPRG();
			}
		});
		panel_1.add(btnRun);

		JButton btnEdit = new JButton("EDIT");
		btnEdit.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				editPRG();
			}
		});
		panel_1.add(btnEdit);

		JButton btnNewButton = new JButton("LIST");
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				listPRG();
			}
		});
		panel_1.add(btnNewButton);

		textField_PRG = new JTextField();
		panel_1.add(textField_PRG);
		textField_PRG.setColumns(10);

		textField = new JTextField();
		textField.addKeyListener(new KeyAdapter() {
			@Override
			public void keyReleased(KeyEvent e) {
				if (e.getKeyCode() == KeyEvent.VK_UP) {
					if (history.size() < 1) {
						return;
					}
					textField.setText(history.get(history.size() - 1));
				}
			}
		});
		textField.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				try {
					Terminal.getInstance().sendCommand(textField.getText());
					history.add(textField.getText());
					textField.setText("");
				} catch (Exception ex) {
					ex.printStackTrace();
				}
			}
		});
		frame.getContentPane().add(textField, BorderLayout.SOUTH);
		textField.setColumns(10);
	}

	protected String getPRGname() {
		String txt = textField_PRG.getText();
		if (txt == null || txt.isEmpty()) {
			txt = null;
			console("(!!) Need a PRG name !");
		}
		return txt;
	}

	protected JScrollPane getScrollPane() {
		return scrollPane;
	}
}
