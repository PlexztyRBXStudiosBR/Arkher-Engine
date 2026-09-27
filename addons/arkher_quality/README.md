# Arkher Quality (addon) — v0.2

**ArkherQualityDirector** — a base do pilar "fotorreal em qualquer celular".
Pure GDScript: **não precisa compilar a engine** — funciona no Godot estável
(4.3/4.4+) E no fork Arkher (4.8-dev).

## O que ele faz

1. **Resolução dinâmica** com histerese: mede o FPS real; se ficar abaixo do
   alvo por `down_after_seconds`, baixa a escala de render (`step_down`);
   se ficar acima por `up_after_seconds`, sobe (`step_up`). Escala fica entre
   `min_scale` e `max_scale` (padrão 0.4..1.0).
2. **Upscale com FSR/FSR2** quando o engine oferece (4.4+ e fork); senão,
   upscale bilinear.
3. **Tiers de qualidade** (0 low / 1 med / 2 high) derivados da escala —
   ligam/desligam: sombras (por luz, com distância máx.), SDFGI, SSAO, SSIL,
   glow, fog volumétrico, SSR, reflexos, MSAA. Tudo configurável por tier
   no inspector (arrays `[low, medium, high]`).
4. **Overlay de debug** (canto superior esquerdo): `ARKHER fps | scale |
   tier | alvo` — essencial para ver a auto-quality agindo no celular.

## Como usar

1. Ative o plugin **ArkherQuality** (Project Settings → Plugins).
2. Adicione um nodo **ArkherQualityDirector** na cena raiz.
3. Selecione o `WorldEnvironment` do jogo na propriedade `world_environment`.
4. Ajuste `target_fps` (ex.: 60), `min_scale` (ex.: 0.4).
5. Pronto. Sinais: `scale_changed(scale)`, `tier_changed(tier)`,
   `quality_changed(scale, tier)`.

## Teste rápido no PC

Abra `addons/arkher_quality/demo_quality.tscn` → botão **Play**.
Para ver a auto-quality trabalhar: no project settings, reduza o FPS do
editor (Editor → Settings → Interface → ... ou use `OS.set_low_processor_usage_mode(true)`
num script temporário), ou baixe o `target_fps` para 20 — a escala vai
subir; suba para 120 — ela cai.

## Teste no Itel A70 (dispositivo de referência)

1. Godot → Editor → Manage Export Templates → baixe os templates.
2. Export → Android: crie/insira um keystore de debug, marque `one_click_deploy`.
3. Export para `res://demo.apk`, instale no A70 (USB debugging).
4. Rode a cena `demo_quality.tscn` — o overlay mostra fps/scale/tier em
   tempo real no celular.
5. **Critério do tier Mobile (ROADMAP M2):** a demo segura ≥55fps no A70
   com tier LOW/MED e escala entre 0.4 e 0.7 (720p nativo).

## Limitações da v0.2 (próximos passos)

- Troca de tier reescreve propriedades do `Environment` — ok para 1
  WorldEnvironment; cenas com vários ambientes exigem a lista de ambientes
  (M2: `ArkherQualityDirector` em C++ com registro de ambientes).
- Sombras por varredura de luzes: ok até ~50 luzes; C++ fará via
  `RenderingServer` em lote.
- No M2 a versão C++ adiciona: teto térmico (leitura do SO), integração com
  o futuro renderer Arkher e presets como `Resource` salvos no projeto.
