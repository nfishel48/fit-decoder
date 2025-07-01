defmodule FitDecoder.MixProject do
  use Mix.Project

  def project do
    [
      app: :fit_decoder,
      version: "0.1.0",
      elixir: "~> 1.18",
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      compilers: [:elixir_make] ++ Mix.compilers(),
      description:
        "A high-performance Elixir library for decoding Garmin .fit files using a C++ NIF.",
      package: package(),
      docs: [
        main: "FitDecoder",
        extras: ["README.md", "FIELDS.md", "HELPER_FUNCTIONS.md"]
      ]
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger],
      mod: {FitDecoder.Application, []}
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      {:elixir_make, "~> 0.9", runtime: false},
      {:ex_doc, ">= 0.0.0", only: :dev, runtime: false}
    ]
  end

  defp package do
    [
      files:
        ~w(lib c_src Makefile .formatter.exs mix.exs README.md FIELDS.md HELPER_FUNCTIONS.md),
      licenses: ["Apache-2.0"],
      links: %{
        "GitHub" => "https://github.com/nfishel48/fit-decoder"
      }
    ]
  end
end
