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

  const handleEditorWillMount = (monaco: typeof import("monaco-editor")) => {
    monaco.languages.register({ id: "rasm" });

    monaco.languages.setLanguageConfiguration("rasm", {
      wordPattern: /[_\w]+/,
    });

    monaco.languages.registerCompletionItemProvider("rasm", {
      provideCompletionItems: (model, position) => {
        const word = model.getWordUntilPosition(position);
        const range = {
          startLineNumber: position.lineNumber,
          endLineNumber: position.lineNumber,
          startColumn: word.startColumn,
          endColumn: word.endColumn,
        };

        const makeSnippet = (label: string, count: number, doc: string = "") => ({
          label,
          kind: monaco.languages.CompletionItemKind.Keyword,
          insertText: `${label}${count > 0 ? " " + Array.from({ length: count }, (_, i) => `\${${i + 1}:ARG${i + 1}}`).join(", ") : ""}`,
          insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
          documentation: doc,
          range,
        });

        const suggestions = [
          makeSnippet("MOVI", 2),
          makeSnippet("MOVR", 2),
          makeSnippet("ADD", 3),
          makeSnippet("SUB", 3),
          makeSnippet("MUL", 3),
          makeSnippet("DIV", 3),
          makeSnippet("MOD", 3),
          makeSnippet("AND", 3),
          makeSnippet("OR", 3),
          makeSnippet("XOR", 3),
          makeSnippet("NOT", 2),
          makeSnippet("CMP", 2),
          makeSnippet("JE", 1),
          makeSnippet("JNE", 1),
          makeSnippet("JL", 1),
          makeSnippet("JLE", 1),
          makeSnippet("JG", 1),
          makeSnippet("JGE", 1),
          makeSnippet("JMP", 1),
          makeSnippet("CEQ", 1),
          makeSnippet("CNE", 1),
          makeSnippet("CL", 1),
          makeSnippet("CLE", 1),
          makeSnippet("CG", 1),
          makeSnippet("CGE", 1),
          makeSnippet("CALL", 1),
          makeSnippet("RET", 0),
          makeSnippet("HALT", 0),
          makeSnippet("PUSH", 1),
          makeSnippet("POP", 1),
          makeSnippet("SHL", 3),
          makeSnippet("SHR", 3),
          makeSnippet("SAR", 3),
          makeSnippet("ROL", 3),
          makeSnippet("ROR", 3),
          makeSnippet("LOAD", 2),
          makeSnippet("STORE", 2),
          makeSnippet("LOADR", 2),
          makeSnippet("LOADB", 2),
          makeSnippet("PRINT", 2),
          makeSnippet("DRAW", 0),
          makeSnippet("INITDISPLAY", 0),
          makeSnippet("STRB", 3),
          makeSnippet("CLS", 0),
          makeSnippet("ALLOWMOD", 0),
          makeSnippet("DISABLEMOD", 0),
          makeSnippet("SPRB", 3),
          makeSnippet("CLSM", 2),
          makeSnippet("POLL", 0),
          makeSnippet("RDI", 2),
          makeSnippet("STD", 1),
          makeSnippet("STDI", 1),
          makeSnippet("RTD", 1),
          makeSnippet("STS", 1),
          makeSnippet("STSI", 1),
          makeSnippet("STRS", 3),
          makeSnippet("DSTR", 1),
          makeSnippet("LDS", 2),
        ];

        return { suggestions };
      },
    });

    monaco.languages.setMonarchTokensProvider("rasm", {
      tokenizer: {
        root: [
          [/;.*/, "comment"],
          [/\b[A-Z0-9]+\b/, "keyword"],
          [/\b_\w+\b/, "custom-underscore"],
          [/\b0x[0-9A-Fa-f]+\b/, "number"],
          [/\b\d+\b/, "number"],
          [/[a-zA-Z_]\w*/, "identifier"],
          [/\s+/, "white"],
        ],
      },
    });

    monaco.editor.defineTheme("rasm-theme", {
      base: "vs-dark",
      inherit: true,
      rules: [
        { token: "comment", foreground: "6C757D", fontStyle: "italic" },
        { token: "keyword", foreground: "FF8800", fontStyle: "bold" },
        { token: "custom-underscore", foreground: "FF0000" },
        { token: "number", foreground: "00FFFF" },
        { token: "identifier", foreground: "FFFFFF" },
      ],
      colors: {
        "editor.background": "#1e1e1e",
        "editor.foreground": "#FFFFFF",
      },
    });
  };

  const writeFile = async () => {
    try {
      await invoke("write_file", { contents: value });
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
        <div className={`fixed transition-all duration-150 bg-neutral-900 border-t-[1px] border-neutral-600 max-h-[20vh] pb-2 overflow-y-auto ${isOutputDown ? "min-h-[30px] h-[35px]" : "min-h-[250px]"} z-[10000] bottom-0 w-full`}>
          <div className="sticky top-0 left-0 w-full bg-neutral-800 text-white flex items-center justify-end px-5 h-[30px]">
            <p className="mr-auto opacity-50 text-sm">Output</p>
            <button className="p-2 opacity-50" onClick={() => setIsOutputDown(prev => !prev)}>
              {isOutputDown ? <ChevronUp size={20} /> : <ChevronDown size={20} />}
            </button>
          </div>
          <pre className="mt-1 p-2 text-green-300 rounded text-sm whitespace-pre-wrap">{output}</pre>
        </div>
      )}
      <div className="flex gap-2 px-7 py-2 bg-neutral-900">
        <button onClick={writeFile} className="px-4 py-1 bg-neutral-700 border-[1px] border-neutral-500 text-white rounded">
          Save
        </button>
        <button onClick={runBinary} className="px-4 py-1 bg-neutral-700 border-[1px] border-neutral-500 text-white rounded">
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
          stickyScroll: { enabled: false },
          minimap: { enabled: false },
        }}
      />
    </>
  );
};

export default CodeContent;
