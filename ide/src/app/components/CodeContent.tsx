"use client";

import { Editor } from "@monaco-editor/react";
import { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/core";
import { listen } from "@tauri-apps/api/event";
import { ChevronDown, ChevronUp } from "lucide-react";

const CodeContent = () => {
  const [value, setValue] = useState<string>("");
  const [output, setOutput] = useState<string>("");
  const [isOutputDown, setIsOutputDown] = useState<boolean>(false);

  const readFile = async () => {
    try {
      const contents = await invoke<string>("read_file");
      setValue(contents);
    } catch (err) {
      console.error("Failed to read file:", err);
    }
  };

  useEffect(() => {
    readFile();

    const setupListener = async () => {
      await listen<string>("vm-output", (event) => {
        setOutput((prev) => prev + "\n" + event.payload);
      });
    };

    setupListener();
  }, []);

  // Setup language and theme synchronously before monaco mounts
  const handleEditorWillMount = (monaco: typeof import("monaco-editor")) => {
    monaco.languages.register({ id: "rasm" });


    monaco.languages.setMonarchTokensProvider("rasm", {
      tokenizer: {
        root: [
          // Comments (from ; to end)
          [/;.*/, "comment"],

          // Commands/keywords: all uppercase words (A-Z and digits), no lowercase
          [/\b[A-Z0-9]+\b/, "keyword"],

          // Words starting with _
          [/\b_\w+\b/, "custom-underscore"],

          // Numbers (hex and decimal)
          [/\b0x[0-9A-Fa-f]+\b/, "number"],
          [/\b\d+\b/, "number"],

          // Identifiers/labels (anything else)
          [/[a-zA-Z_]\w*/, "identifier"],

          // Whitespace
          [/\s+/, "white"],
        ],
      },
    });

    monaco.editor.defineTheme("rasm-theme", {
      base: "vs-dark",
      inherit: true,
      rules: [
        { token: "comment", foreground: "6C757D", fontStyle: "italic" }, // dull gray italic
        { token: "keyword", foreground: "FF8800", fontStyle: "bold" },   // distinct orange/red bold
        { token: "custom-underscore", foreground: "FF0000" },            // red for _words
        { token: "number", foreground: "00FFFF" },                       // cyan for numbers
        { token: "identifier", foreground: "FFFFFF" },                   // white for normal text
      ],
      colors: {
        "editor.background": "#1e1e1e",
        "editor.foreground": "#FFFFFF",
      },
    });

    // monaco.languages.setMonarchTokensProvider("rasm", {
    //   tokenizer: {
    //     root: [
    //       [/\b_\w+\b/, "custom-underscore"],
    //       [/.+/, ""],
    //     ],
    //   },
    // });
    //
    // monaco.editor.defineTheme("rasm-theme", {
    //   base: "vs-dark",
    //   inherit: true,
    //   rules: [
    //     { token: "custom-underscore", foreground: "FF0000" }, // red for _words
    //     { token: "", foreground: "FFFFFF" }, // white for default
    //   ],
    //   colors: {
    //     "editor.background": "#1e1e1e",
    //     "editor.foreground": "#FFFFFF",
    //   },
    // });
  };

  const writeFile = async () => {
    try {
      await invoke("write_file", { contents: value });
      console.log("File written successfully");
    } catch (err) {
      console.error("Failed to write file:", err);
    }
  };

  const runBinary = async () => {
    try {
      setOutput("");
      await invoke("run_binary", { contents: value });
    } catch (err) {
      setOutput(`Error: ${String(err)}`);
    }
  };

  return (
    <>
      {output && (
        <div
          className={`fixed transition-all duration-150 bg-neutral-900 border-t-[1px] border-neutral-600 max-h-[20vh] pb-2 overflow-y-auto ${isOutputDown ? "min-h-[30px] h-[35px]" : "min-h-[250px]"
            } z-[10000] bottom-0 w-full`}
        >
          <div className="sticky top-0 left-0 w-full bg-neutral-800 text-white flex items-center justify-end px-5 h-[30px]">
            <p className="mr-auto opacity-50 text-sm">Output</p>
            <button
              className="p-2 opacity-50"
              onClick={() => {
                setIsOutputDown((prev) => !prev);
              }}
            >
              {isOutputDown ? <ChevronUp size={20} /> : <ChevronDown size={20} />}
            </button>
          </div>
          <pre className="mt-1 p-2 text-green-300 rounded text-sm whitespace-pre-wrap">{output}</pre>
        </div>
      )}
      <div className="flex gap-2 px-7 py-2 bg-neutral-900">
        <button
          onClick={writeFile}
          className="px-4 py-1 bg-neutral-700 border-[1px] border-neutral-500 text-white rounded"
        >
          Save
        </button>
        <button
          onClick={runBinary}
          className="px-4 py-1 bg-neutral-700 border-[1px] border-neutral-500 text-white rounded"
        >
          Run
        </button>
      </div>
      <Editor
        defaultLanguage="rasm"
        value={value}
        className="h-[100vw]"
        onChange={(val) => setValue(val || "")}
        theme="rasm-theme"
        beforeMount={handleEditorWillMount}
        options={{
          fontSize: 16,
          stickyScroll: {
            enabled: false,
          },
          minimap: { enabled: false },
        }}
      />
    </>
  );
};

export default CodeContent;
