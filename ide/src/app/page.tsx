import Image from "next/image";
import CodeContent from "./components/CodeContent";

export default function Home() {
  return (
    <>
      <main className="relative max-h-[100vh] overflow-y-clip">
        <CodeContent />
      </main>
    </>
  );
}
